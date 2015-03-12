The one-stop v8-juice/v8-convert Table of Contents...




## v8-convert ##

v8-convert is a mid-2011 spinoff of v8-juice, providing a streamlined, header-only library which focuses solely on type conversions.

  * [Introduction/overview](V8Convert.md) of v8-convert's features.
  * [Summary of changes](ChangeLog.md)
  * [Contributing](Contributing.md) to v8-juice and v8-convert.
  * How to [bind native function/methods](V8Convert_FunctionBinding.md) to JS.
    * How to [overload function and constructor bindings](V8Convert_Overloading.md).
  * How to [bind native non-function variables](V8Convert_PropertyBinding.md) to JS.
  * An [alternative interface](V8Convert_XTo.md) for function and property bindings.
  * Various [Tips, tricks, and how-tos](V8Convert_TipsAndTricks.md).
  * Addons/plugins/demos:
    * Yet another [class-binding mechanism](V8Convert_ClassCreator.md)
    * [ByteArray class](V8Convert_ByteArray.md)
    * [V8Shell](V8Convert_Shell.md) simplifies bootstrapping of v8 in an application.
    * [Posix-like socket API](V8Convert_Socket.md)
    * [JSPDO database access](JSPDO.md) abstraction layer
      * [JSPDO\_API](JSPDO_API.md)
      * [JSPDO\_HowTo](JSPDO_HowTo.md)

## v8-juice ##

v8-juice is the "main project". It has been online since early 2009 and has accumulated quite a bit of code and wiki pages...

  * [Home page](HomePage.md)
  * [Build and installing](BuildingTheCode.md)
  * [Summary of changes](ChangeLog.md)
  * [Contributing](Contributing.md) to v8-juice and v8-convert
  * [Binding native functions](BindingFunctions.md)
  * Binding native classes:
    * [Intro to binding classes](CreatingJSClasses.md) using v8-juice.
    * [ClassBinder](ClassBinder.md) - the libirary's first generation of class binding API.
      * [ClassBinderPartTwo](ClassBinderPartTwo.md)
    * [ClassWrap](ClassWrap.md) - 2nd-generation wrapper.
      * [ClassWrap policies](ClassWrapPolicies.md)
  * [Basic multi-threading](ThreadingInJS.md) in JS scripts.
  * [Plugins](Plugins.md):
    * [PathFinderClass](PathFinderClass.md)
    * [Curl](PluginCurl.md)
    * [XML parser](PluginExpatParser.md)
    * [ncurses](PluginNCurses.md)
    * [File class](PluginPosixFILE.md)
    * [poll()](PluginPosixPoll.md)
    * [readline](PluginReadline.md)
    * [sprintf()](PluginSprintf.md)
    * [sqlite3](PluginSQLite3.md)
    * [whefs](PluginWhefs.md) (embedded filesystem)
    * [whio](PluginWhio.md) (i/o library)
  * [WhereToFindMoreDocs](WhereToFindMoreDocs.md)