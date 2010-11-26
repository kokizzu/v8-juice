var echo = {
host:'localhost',//127.0.0.1',
//port:9090,
port:8080,
crnl:'\r\n'
};
loadPlugin("v8-juice-Socket",echo);
echo.socketType =
    //echo.Socket.SOCK_DGRAM
    echo.Socket.SOCK_STREAM
    ;
echo.socketFamily = echo.Socket.AF_INET;
