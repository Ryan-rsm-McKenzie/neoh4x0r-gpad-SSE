Scriptname Controller Hidden

; valid positions are from -32767 to 32767
; you want the direction in form -1 to 1 (divide the result by 32768.0)

; valid directions are as follows
;
;	enum
;	{
;		kDiretion_none		= 0,
;		kDiretion_up		= 1,
;		kDiretion_upright	= 2,
;		kDiretion_right		= 3,
;		kDiretion_downright	= 4,
;		kDiretion_down		= 5,
;		kDiretion_downleft	= 6,
;		kDiretion_left		= 7,
;		kDiretion_upleft	= 8,		
;	};
;
	
; returns the left-stick position from right to left
float Function GetLX() global native

; returns the left-stick position from up to down
float Function GetLY() global native

; returns the right-stick position from right to left
float Function GetRX() global native

; returns the right-stick position from up to down
float Function GetRY() global native

; returns the left stick's direction
int Function GetLSDirection() global native

; returns the left stick's direction
int Function GetRSDirection() global native

; Will toggle the state of the gamepad from enabled to disabled
Function ToggleGamepad() global native