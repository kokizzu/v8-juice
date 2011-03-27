
function test2() {
    var s = v8juice.getStackTrace();
    print(JSON.stringify(s,undefined,'\t'));
}
function test1() {
    new test2();
}
test1();

