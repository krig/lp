#pragma once
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/inotify.h>


struct Notify {
	static const size_t MIN_READ_SIZE = sizeof(inotify_event) + NAME_MAX + 1;
	static const size_t BUF_SIZE = MIN_READ_SIZE * 8;

	Notify() {
		_fd = inotify_init1(IN_NONBLOCK);
		_fill = 0;
		memset(_buf, 0, BUF_SIZE);
	}

	~Notify() {
		if (_fd >= 0)
			close(_fd);
	}

	template <typename CB>
	void update(CB cb) {
		for (;;) {
			if (_fill < _eventlen())
				_refill();
			if (_fill < _eventlen())
				return;
			cb(_event());
			_next();
		}
	}

	int add(const char* pathname, uint32_t mask = IN_CREATE | IN_DELETE | IN_CLOSE_WRITE) {
		if (_watches.find(pathname) != _watches.end())
			return _watches[pathname];
		int wd = inotify_add_watch(_fd, pathname, mask);
		if (wd == -1)
			throw error("inotify_add_watch: %s", strerror(errno));
		_watches[pathname] = wd;
		return wd;
	}

private:
	inotify_event* _event() {
		return &_eventobj;
	}

	size_t _eventlen() const {
		if (_fill < sizeof(inotify_event))
			return sizeof(inotify_event);
		return sizeof(inotify_event) + _eventobj.len;
	}

	void _refill() {
		ssize_t rc = read(_fd, _buf + _fill, BUF_SIZE - _fill);
		if (rc < 0) {
			if (errno == EAGAIN || errno == EWOULDBLOCK)
				return;
			if (errno == EINTR)
				return;
			throw error("notify: %s", strerror(errno));
		}
		LOG_INFO("read returned %ld (fill=%ld)", rc, _fill);
		_fill += rc;
	}

	void _next() {
		size_t eventlen = _eventlen();
		if (eventlen > _fill)
			throw error("eventlen > fill");
		LOG_INFO("_next (eventlen=%ld, fill=%ld)", eventlen, _fill);
		memmove(_buf, _buf + eventlen, _fill - eventlen);
		_fill -= eventlen;
	}

	int _fd;
	size_t _fill;
	union {
		char _buf[BUF_SIZE];
		inotify_event _eventobj;
	};
	dict<string, int> _watches;
};
