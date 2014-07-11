# changelog

This file contains highlights of what changes on each version of the
polymer_expressions package.

#### Pub version 0.12.0-dev
  * Updated to depend on latest template_binding and observe. Setting a value on
    a polymer expression binding now produces a change notification.

#### Pub version 0.11.0
  * Remove faulty assert that threw when an iterable field was updated.
  
#### Pub version 0.11.0
  * Scopes created by templates are created less often and nested properly. The
    previous version had a bug with respect to the names visible in 
    <template releat> tags without an "in" expression, and <template bind> tags.
    In those templates, names for the outer templates should not be visible.
    This may result in some breakages in templates that relied on the buggy
    behavior.
  * <template bind> now supports "as" expressions.
  * Removed warnings when attempting to assign a value to a property on null
    object, or assign a value to a non-assignable expression. Polymer binding
    initialization sometimes assigns to expressions, so this should reduce
    unecessary warnings.
  * Added the % (modulo), === (identical) and !== (not identical) operators.
  * Fast-path for eval(). eval() no longer observes expressions or creates a
    tree of observers.
  * PolymerExpressions bindings clean up expression observers when closed,
    fixing a potential memory leak.
  * Better parse errors. Unknown operators are reported, and all exceptions are
    instances of ParseException so that they can be caught independently of
    exceptions generated by calling user code.


#### Pub version 0.10.0
  * package:polymer_expressions no longer declares @MirrosUsed. The package uses
    mirrors at development time, but assumes frameworks like polymer will
    generate code that replaces the use of mirrors. If you use this directly,
    you might need to do code generation as well, or add the @MirrorsUsed
    declaration. This can be done either explicitly or by importing the old
    settings from 'package:observe/mirrors_used.dart' (which include
    @reflectable and @observable by default).

  * Errors that occur within bindings are now thrown asycnhronously. We used to
    trap some errors and report them in a Logger, and we would let other errors
    halt the rendering process. Now all errors are caught, but they are reported
    asynchornously so they are visible even when logging is not set up.

  * Fixed several bugs, including:
      * propagating list changes ([18749][]).
      * precedence of ternary operators ([17805][]).
      * two-way bindings ([18410][] and [18792][]).

[17805]: https://code.google.com/p/dart/issues/detail?id=17805
[18410]: https://code.google.com/p/dart/issues/detail?id=18410
[18749]: https://code.google.com/p/dart/issues/detail?id=18749
[18792]: https://code.google.com/p/dart/issues/detail?id=18792