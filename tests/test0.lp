# this is a comment

foo :: (a: int, b: int) -> int {
    return a + b;
}

varargs :: (a: int, ...) -> () {
    for a {
        println("{}", it);
    }
}

main :: () {
    a := 3;
    b := 4.5; # another comment
    c : float = b + b * 9;
    d := "this is a \"string";
    c = b / (a + 100.f);
    return 0;
}
