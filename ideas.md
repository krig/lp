# lp

## basics

I haven't decided how to represent pointers in my language. I want to
make it easy to avoid using null as a special value and instead use a
binary variant type and type matching.

## basic types

* u8-128
* i8-128
* f32, f64
* int = i32
* float = f32
* str - immutable, stored as utf-8 { nbytes, nchars, data[] } in memory
* array - stored as { capacity, length, data[] } in memory
* bool = true|false, storage depends (can be packed into single byte in
  structs, or a full i32 (haven't decided on this yet) )
* pointers as in C


## functions

first class citizens. syntax?


    f := (x:int, y:int) -> int {
        return x + y;
    }

    identity := ()->() {};

also

    identity := {}; # block form


## blocks

blocks are bodies in functions, or free-standing. blocks have a
capture, which determines what variables from the outer scope are
visible and how they are referred (in case the lifetime of the block
is longer than the outer scope). Scopes are no-argument, no-return
functions unless created as function bodies (if stored in a variable).

## hashtable

hash tables and arrays should be native types (maybe also sets).
generic types?

f := [int: str; 3: "hello", 4: "goodbye"];

## better enums

each enum value has attributes like name (str), the enum itself has a
count attribute, enum values are accessed like so:

f :: enum { a, b, c, d };
v := f.a;
println("%s = %d", f.a.name, f.a);

enum is basically sugar for a struct with some additional
functionality... an enum should be an iterable as well.

There's some odd magic that you'd want happen with enum where you can
use an enum value wherever you normally can use an integer, without
subscripting. But that's fine, in C++ that's even something you can
define yourself with conversion operators. I'm not sure I want to
allow operator overloading yet..

e :: enum { a b c d };
for e {
  println("{} = {}", it.name, it);
}

## structs

pretty much just like c, although no variable thingie so no semicolon

struct foo {
  blah: int
}

oh, and everything is set to 0/null by default

http://macton.smugmug.com/gallery/8936708_T6zQX#!i=593426709&k=ZX4pZ

* distinguish between ro, wo, rw
* enable restrict / no aliasing wherever possible


## lazy overloading / inference

Type inference is context-specific, which enables a type of generics
and makes the conversion rules for constants more lenient. I think
this works well in practice. It also makes sense both for variables
and functions. So in the definition of

    min :: (a, b) {
      (b < a) ? b : a
    }

The types of a and b (and the return type) are all undetermined until
the function is used, at which point a and b are typed based on the
arguments...

Naming such types? Makes sense, so you can declare other variables of
the same type, or declare that certain variables should have the same
type.

    min :: (a: 't, b: 't) -> 't {
      (b < a) ? b : a
    }

So ' in this language is used to define names of types. The names are
local, so for two functions that both declare variables of type 'foo,
the 'foo's are not related.

also, the type can be different in different scopes? so maybe if this
branch is taken, the type is evaluated to int, whereas if we take this
other branch, the type is evaluated to float. This would be a runtime
difference... that would be cool, but not crucial for the first
version.

Do I want the rust style of semicolon as statement separator instead
of statement terminator, and implicit return of last statement in
scope?

## namespacing / scopes

Do I want modules more like python or namespaces like C++? The
namespace should be a local thing, so that the library/module doesn't
declare a name for itself, but when imported a module gets put in a
namespace which can either get its name from the module name, or from
the import statement. So

use "github.com/krig/lp-gl@1.0.0" as gl;
use "github.com/krig/lp-sdl2@1.0.0" as sdl2;

work because there is no module name, but the namespace becomes
gl/sdl2.

for C shared libraries or C symbols in general you do need a module
definition with ffi declarations and a version number and so on.

main :: () {
}


## unified function call syntax

allow calling functions as arg1.function(arg2, ...);
"function" will be looked up in the namespace where arg1 is defined.

namespaces are open, so to add a "method" to string for example, just
define it in the std namespace;

namespace std {
  basename :: (string s) -> string {
    # ....
  }
}


## on lambdas and types

So, one problem with having lambdas in manually memory managed
language is that the function type and object size varies with the
size of the capture. In C++, this is solved by having the type
generated and requiring lambdas to be referenced using auto, and
combining that with templates which enables writing functions that
take functions as parameters that can take objects of any size as
suggogates.

I don't want to have templates in the sense that C++ does them, but I
do want lambdas with capture. I'm not entirely sure how to resolve
this yet.

I might want to have a "function" placeholder type which would be
resolved at compile time, similar to how templates work. So if you
declare a variable to be of function type but without making it
immutable and providing a value, the type is actually undetermined
until provided with a context. The problem with that is handling such
ambiguous function references as values.

I think maybe something like Go interfaces could work there. Instead
of a function value being a pointer, it's a pair of pointers: One that
identifies the type of the function, and one that points to the
function or capture. Of course that would mean that the compiler would
have to memory manage such values. Actually I wouldn't be too much
against using reference counters for that purpose. It would slow
things down, but acceptably so I think.

