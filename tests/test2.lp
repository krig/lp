# this is another attempt to write a simple program
# in this fictional language.
use "github:krig/lp-std @master" as std;
use "github:krig/lp-gl @master" as gl;
use "github:krig/lp-sdl2 @master" as sdl;

# this is in std of course
def exit(v) {
     match v {
     0 -> _cexit(0);
     string ->
       os.stderr.println("{}", v);
       _cexit(1);
     int -> _cexit(v);
     _ -> static_assert("Invalid argument type {} {}", type(v), v);
     }
}

def apply_using(ast_using: &Ast_Using) {
  while ast_using.identifiers {
    ident :: ast_using.identifiers.pop();
    if !ident.resolved_declaration {
       interp.report_error(ast_using, "internal compiler error\n", ident.name.name);
       return;
    }
    match ident.resolved_declaration.type {
    case Ast_Expr.Description -> ;
    case Ast_Expr.Enum -> ;
    case _ ->
         interp.report_error(ident, "blah blah\n");
    }
  }
}

def main() {
     if sdl.Init(sdl.INIT_EVERYTHING) < 0 then std.exit("Failed to init SDL");
    window :: sdl.CreateWindow("test2", sdl.WINDOWPOS_UNDEFINED, sdl.WINDOWPOS_UNDEFINED,
        640, 480, sdl.WINDOW_OPENGL + sdl.WINDOW_RESIZABLE);
    unless window then std.exit(sdl.GetError());
    context :: window.GL_CreateContext();
    unless context then std.exit(sdl.GetError());
    window.GL_MakeCurrent(context);
    
    0
}
