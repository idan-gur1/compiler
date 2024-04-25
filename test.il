Function outputString  MaxTemp: 0 Params: str 
ScopeEnter Vals: none
outputStringWhile1:
Goto outputStringWhile1Condition
outputStringWhile1Body:
ScopeEnter Vals: none
PushParam str[0]
Call outputChar
str = &str[1]
ScopeExit
outputStringWhile1Condition:
GotoIfNotZero str[0] outputStringWhile1Body
ScopeExit
outputStringEnd:
EndFunction
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
Function infRec  MaxTemp: 1 Params: n 
ScopeEnter Vals: none
temp1 := n - 1
PushParam temp1
temp1 := RetValOf Call infRec
SetReturnValue temp1
Goto infRecEnd
ScopeExit
infRecEnd:
EndFunction
Function main  MaxTemp: 1 Params: none
ScopeEnter Vals: arr ptr name 
arr[0] = 15
arr[1] = 8
PushParam 8
temp1 := RetValOf Call fib
arr[2] = temp1
arr[3] = 40
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
name[0] = 105
name[1] = 100
name[2] = 97
name[3] = 110
name[4] = 32
name[5] = 103
name[6] = 117
name[7] = 114
name[8] = 0
PushParam &name
Call outputString
PushParam 10
Call outputChar
PushParam &name
Call outputString
PushParam 10
Call outputChar
SetReturnValue ptr[0]
Goto mainEnd
ScopeExit
mainEnd:
EndFunction
