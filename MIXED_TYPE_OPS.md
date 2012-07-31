JavaScript Operations on Mixed Types
====================================

Plus (+) Operator
-----------------
Number + Number       => Number (or Infinity)
Number + String       => String (stringify number and concatenate)
Number + null         => Number (where null is 0 in the addition)
Number + undefined    => NaN
Number + Boolean      => Number (where bool is 0 or 1 in the addition)
Number + Object       => String (stringify number and concatenate with Object.toString())
String + String       => String
String + null         => String (null becomes 'null' and concatenate)
String + undefined    => String (undefined becomes 'undefined' and concatenate)
String + Boolean      => String (stringify bool and concatenate)
String + Object       => String (concatenate with Object.toString())
Boolean + Boolean     => Number (where bool is 0 or 1 in the addition)
Boolean + null        => Number (addition -- bool is 0 or 1, null is 0)
Boolean + undefined   => NaN
Boolean + Object      => String (stringify bool, Object.toString())
Object + Object       => String (concatenate Object.toString() of both)
Object + null         => String (null becomes 'null', Object.toString())
Object + undefined    => String (undefined becomes 'undefined', Object.toString())
null + null           => Number (always 0)
null + undefined      => NaN
undefined + undefined => NaN

Minus (-) Operator
------------------
Number - Number       => Number (or Infinity)
Number - String       => Number or NaN
Number - null         => Number (always the number itself)
Number - undefined    => NaN
Number - Boolean      => Number
Number - Object       => NaN
String - String       => NaN
String - null         => NaN
String - undefined    => NaN
String - Boolean      => NaN
String - Object       => NaN
Boolean - null        => Number (value of bool)
Boolean - undefined   => NaN
Boolean - Object      => NaN
Object - Object       => NaN
Object - null         => NaN
Object - undefined    => NaN
null - undefined      => NaN
null - null           => Number (always 0)
undefined - undefined => NaN

Times (\*) Operator
-------------------
Number * Number       => Number
Number * String       => Number or NaN
Number * null         => Number (value of Number)
Number * undefined    => NaN
Number * Boolean      => Number
Number * Object       => NaN
String * String       => NaN (or Number)
String * null         => 
String * undefined
String * Boolean
String * Object
Boolean * null
Boolean * undefined
Boolean * Object
Object * Object
Object * null
Object * undefined
null * undefined
null * null
undefined * undefined

Division (/) Operator
---------------------
Number / Number
Number / String
Number / null
Number / undefined
Number / Boolean
Number / Object
String / String
String / null
String / undefined
String / Boolean
String / Object
Boolean / null
Boolean / undefined
Boolean / Object
Object / Object
Object / null
Object / undefined
null / undefined
null / null
undefined / undefined

Modulus (%) Operator
--------------------
Number % Number
Number % String
Number % null
Number % undefined
Number % Boolean
Number % Object
String % String
String % null
String % undefined
String % Boolean
String % Object
Boolean % null
Boolean % undefined
Boolean % Object
Object % Object
Object % null
Object % undefined
null % undefined
null % null
undefined % undefined
