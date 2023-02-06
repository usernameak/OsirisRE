Larian Studios' Osiris VM reverse-engineering project
=====================================================

Based on OsirisDLL from Divine Divinity version 1.0.0.62 (Osiris version 1.4)

Has to be compiled with a 32-bit MSVC ≥ 2019.

DLL wrapper
-----------

Just a bunch of headers and an import library for OsirisDLL.dll.

Compiler wrapper
----------------

A small utility to invoke the compiler in OsirisDLL.dll. Requires the DLL in the same directory.

Common usage is like this (order of arguments matters):

```shell
OsirisCC.exe -log compile.log -compile story.div -save story.000
```

... or, if you want the log to be output into the console:

```shell
OsirisCC.exe -log CON -compile story.div -save story.000
```

Reverse-engineered OsisisDLL.dll
--------------------------------

This is extremely incomplete (only some small parts are currently reverse-engineered), however it might expand in the future to be used as a reference for the decompiler.

About Osiris syntax
-------------------

I'm not exactly sure about the syntax, but one very distinctive feature of Osiris in DD/BD compared to Divinity: Original Sin is that the former has `object`s, declared like this:

```
object { <ObjName>, <type id>, ( <key1>, <key2>, <key3>, <key4> ) }

// <type id> is a type id (numeric one) declared with the following syntax:
type { TYPENAME, ID_NUMBER }

// key1, key2, key3, key4 are similar to the ones used in queries/calls

// for example (random object from the actual game):
type { NPC_CLASS, 9 }
object { NPC_CLASS_Mosquito, 9, (9, 212, 0, 0) }

```