Delay_nms PROC
	    EXPORT Dealy_nms
L1
    SUBS R0, #1
    BCS L1
    BX LR
	ENDP