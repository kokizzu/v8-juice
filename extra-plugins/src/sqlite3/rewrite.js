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

    
    var sql = "create table if not exists t(a,b);";
    var st = new sqlite3.Statement(my.db,sql);
    {
        var curs = new sqlite3.Cursor(st);
        var rc = curs.step();
        print("step() rc=",rc,'errmsg=',my.db.errorMessage());
        curs.close();
    }
    {
        st.prepare("insert into t values(?,?)");
        var curs = //new sqlite3.Cursor(st);
            st.getCursor();
        var now = new Date();
        for( var i = 0; i < 4; ++i )
        {
            st.bindInt( 1, i );
            st.bindText( 2, now+" #"+i );
            var rc = curs.step();
            print("Insert RC =",rc);
            st.clearBindings();
            curs.reset();
        }
        curs.close();
    }
    sql = "select * from t order by rowid desc limit 5";
    st.prepare(sql);
    print("statement:", st);
    print("Prepared?",[st.prepare(sql),st.isPrepared(),my.db.errorMessage()]);
    {
        var sp = sqlite3_prepare( my.db, sql );
        if( ! sp )
        {
            print("Investigate why second prepare fails. Error message is:",
                  sqlite3_errmsg(my.db));
        }
        else
        {
            print("statement 2:", sp);
            print("sqlite3_prepare:", sp);
            sp.finalize();
        }
    }
    var curs = new sqlite3.Cursor(st);
    for( var i = curs.step(); (SQLITE_ROW == (i = curs.step())); )
    {
        print('cursor step #'+i+':',curs.columnName(1),'=',curs.getString(1));
    }
    curs.close();
    st.finalize();
}

tryOne();

print("Shuting down...");
//sqlite3_close( my.db );
my.db.close();
print("Done!");
