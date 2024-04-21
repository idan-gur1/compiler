Function check  MaxTemp: 1 Params: b n 
ScopeEnter Vals: a 
temp1 := b[0] + 4
a = temp1
temp1 := a * 5
temp1 := temp1 + 4
b[1] = temp1
checkWhile1:
Goto checkWhile1Condition
checkWhile1Body:
ScopeEnter Vals: none
temp1 := b[0] * n
b[1] = temp1
temp1 := n - 1
n = temp1
ScopeExit
checkWhile1Condition:
temp1 := n > 0
GotoIfNotZero temp1 checkWhile1Body
ScopeExit
checkEnd:
EndFunction
Function calc  MaxTemp: 2 Params: a b 
ScopeEnter Vals: none
temp1 := a * b
temp2 := a + b
temp1 := temp1 / temp2
SetReturnValue temp1
Goto calcEnd
ScopeExit
calcEnd:
EndFunction
Function getItem  MaxTemp: 0 Params: arr 
ScopeEnter Vals: none
SetReturnValue &arr[2]
Goto getItemEnd
ScopeExit
getItemEnd:
EndFunction
Function fib  MaxTemp: 2 Params: n 
ScopeEnter Vals: none
fibIf1:
temp1 := n < 2
GotoIfZero temp1 fibIf1End
ScopeEnter Vals: none
SetReturnValue 1
Goto fibEnd
ScopeExit
fibIf1End:
temp1 := n - 1
PushParam temp1
temp1 := RetValOf Call fib
temp2 := n - 2
PushParam temp2
temp2 := RetValOf Call fib
temp1 := temp1 + temp2
SetReturnValue temp1
Goto fibEnd
ScopeExit
fibEnd:
EndFunction
Function main  MaxTemp: 1 Params: none
ScopeEnter Vals: intArr cell val arr ptr fibNum 
PushParam &intArr
temp1 := RetValOf Call getItem
PushParam temp1
temp1 := RetValOf Call getItem
cell = temp1
val = -14
temp1 := val * 12
arr[val] = temp1
ptr = &arr
PushParam 12
PushParam &ptr
Call check
PushParam 4
temp1 := RetValOf Call fib
fibNum = temp1
mainWhile1:
mainWhile1Body:
ScopeEnter Vals: digitPlus 
temp1 := arr[0] + 12
arr[0] = temp1
temp1 := val - 1
val = temp1
temp1 := val % 10
temp1 := 1 + temp1
digitPlus = temp1
ScopeExit
mainWhile1Condition:
temp1 := val > 5
GotoIfNotZero temp1 mainWhile1Body
PushParam arr[1]
PushParam arr[0]
temp1 := RetValOf Call calc
arr[2] = temp1
mainIf1:
temp1 := arr[1] > 100
GotoIfZero temp1 mainIf1Else
ScopeEnter Vals: none
SetReturnValue 10
Goto mainEnd
ScopeExit
Goto mainIf1End
mainIf1Else:
ScopeEnter Vals: none
SetReturnValue 0
Goto mainEnd
ScopeExit
mainIf1End:
ScopeExit
mainEnd:
EndFunction
