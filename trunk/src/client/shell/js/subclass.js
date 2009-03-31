/**
   Causes ChildClass to inherit from ParentClass, in the conventional
   OO sense of the word inherit.

   Unlike real-world inheritance, ParentClass must outlive ChildClass.

   Derived from code posted here:

   http://groups.google.com/group/v8-users/browse_thread/thread/adfc2978ee519b42

*/
Object.prototype._extends_ = function( ChildClass, ParentClass )
{
    if( 1 == arguments.length )
    {
	ParentClass = ChildClass;
	ChildClass = this;
    }
    function TempClass() {}
    TempClass.prototype = ParentClass.prototype;

    ChildClass.prototype = new TempClass();
    ChildClass.prototype.constructor = ChildClass;
    ChildClass.prototype._superConstructor_ = ParentClass;
    ChildClass.prototype._superClass_ = ParentClass.prototype;
}

function MyType()
{
    var av = Array.prototype.slice.apply(arguments,[0]);
    print("new MyType(",av.join(','),')');
    this.prop1 = 1;
}

function MySubType()
{
    this._superConstructor_.call( this, Array.prototype.slice.apply(arguments,[0]) );
    var av = Array.prototype.slice.apply(arguments,[0]);
    print("new MySubType(",av.join(','),')');
    this.prop2 = 2;
}

MySubType._extends_( MyType );

var x = new MySubType(7,3,11);

print( x.prop1 + ":" + x.prop2 );
print( x instanceof MyType );

