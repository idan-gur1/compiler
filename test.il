Function fib  MaxTemp: 2 Params: n 
ScopeEnter Vals: none
fibIf1:
temp1 := n == 1
GotoIfZero temp1 fibIf1End
ScopeEnter Vals: none
SetReturnValue 1
Goto fibEnd
ScopeExit
fibIf1End:
fibIf2:
temp1 := n < 1
GotoIfZero temp1 fibIf2End
ScopeEnter Vals: none
SetReturnValue 0
Goto fibEnd
ScopeExit
fibIf2End:
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
Function getMaxPtr  MaxTemp: 1 Params: arr n 
ScopeEnter Vals: i max maxIndex 
i = 1
max = arr[0]
maxIndex = 0
getMaxPtrWhile1:
Goto getMaxPtrWhile1Condition
getMaxPtrWhile1Body:
ScopeEnter Vals: none
getMaxPtrIf1:
temp1 := arr[i]
temp1 := temp1 > max
GotoIfZero temp1 getMaxPtrIf1End
ScopeEnter Vals: none
temp1 := arr[i]
max = temp1
maxIndex = i
ScopeExit
getMaxPtrIf1End:
temp1 := i + 1
i = temp1
ScopeExit
getMaxPtrWhile1Condition:
temp1 := i < n
GotoIfNotZero temp1 getMaxPtrWhile1Body
SetReturnValue &arr[maxIndex]
Goto getMaxPtrEnd
ScopeExit
getMaxPtrEnd:
EndFunction
Function replaceMaxWithZero  MaxTemp: 1 Params: arr n 
ScopeEnter Vals: i max maxIndex 
i = 1
max = arr[0]
maxIndex = 0
replaceMaxWithZeroWhile1:
Goto replaceMaxWithZeroWhile1Condition
replaceMaxWithZeroWhile1Body:
ScopeEnter Vals: none
replaceMaxWithZeroIf1:
temp1 := arr[i]
temp1 := temp1 > max
GotoIfZero temp1 replaceMaxWithZeroIf1End
ScopeEnter Vals: none
temp1 := arr[i]
max = temp1
maxIndex = i
ScopeExit
replaceMaxWithZeroIf1End:
temp1 := i + 1
i = temp1
ScopeExit
replaceMaxWithZeroWhile1Condition:
temp1 := i < n
GotoIfNotZero temp1 replaceMaxWithZeroWhile1Body
arr[maxIndex] = 0
ScopeExit
replaceMaxWithZeroEnd:
EndFunction
Function main  MaxTemp: 1 Params: none
ScopeEnter Vals: arr ptr 
arr[0] = 15
arr[1] = 8
PushParam 8
temp1 := RetValOf Call fib
arr[2] = temp1
arr[3] = 20
arr[4] = 12
arr[5] = 1
arr[6] = 7
arr[7] = 3
PushParam 8
PushParam &arr
Call replaceMaxWithZero
PushParam 8
PushParam &arr
temp1 := RetValOf Call getMaxPtr
ptr = temp1
temp1 := ptr[0] * 2
ptr[0] = temp1
SetReturnValue ptr[0]
Goto mainEnd
ScopeExit
mainEnd:
EndFunction
