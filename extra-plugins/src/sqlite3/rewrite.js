print("Starting up...");

load_plugin('v8-juice-sqlite3');
var my = {db:0,stmt:0,file:"my.db"};

my.db = new sqlite3.DB(my.file);
//my.db = sqlite3_open(my.file);
//print("Using sqlite3 version",sqlite3_libversion());
print('db =',my.db);

function tryOne(){
    print('changes =',sqlite3_changes(my.db));
    print('errmsg =',sqlite3_errmsg(my.db));
    print('errmsg =',my.db.errorMessage());


    var st = new sqlite3.Statement(my.db);
    print("statement:", st);
    var sp = sqlite3_prepare( my.db, "select current_time" );
    print("statement 2:", sp);
    print("sqlite3_prepare:", sp);
    sqlite3_finalize(st);
    sp.finalize();

    //my.db.execute("select current_time");
}

tryOne();

print("Shuting down...");
//sqlite3_close( my.db );
my.db.close();
print("Done!");
