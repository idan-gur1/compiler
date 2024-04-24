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
Function findMaxIndex  MaxTemp: 1 Params: arr n 
ScopeEnter Vals: i max maxIndex 
i = 1
max = arr[0]
maxIndex = 0
findMaxIndexWhile1:
Goto findMaxIndexWhile1Condition
findMaxIndexWhile1Body:
ScopeEnter Vals: none
findMaxIndexIf1:
temp1 := arr[i]
temp1 := temp1 > max
GotoIfZero temp1 findMaxIndexIf1End
ScopeEnter Vals: none
temp1 := arr[i]
max = temp1
maxIndex = i
ScopeExit
findMaxIndexIf1End:
temp1 := i + 1
i = temp1
ScopeExit
findMaxIndexWhile1Condition:
temp1 := i < n
GotoIfNotZero temp1 findMaxIndexWhile1Body
SetReturnValue maxIndex
Goto findMaxIndexEnd
ScopeExit
findMaxIndexEnd:
EndFunction
Function main  MaxTemp: 1 Params: none
ScopeEnter Vals: arr 
arr[0] = 15
arr[1] = 8
arr[2] = 6
arr[3] = 20
arr[4] = 12
arr[5] = 1
arr[6] = 7
arr[7] = 3
PushParam 8
PushParam &arr
temp1 := RetValOf Call findMaxIndex
SetReturnValue temp1
Goto mainEnd
ScopeExit
mainEnd:
EndFunction
