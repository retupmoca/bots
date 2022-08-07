	// set base register for I/O
	lui x31, 0xf0000
	addi x1, x0, 1

	// throttle 100
	addi x2, x0, 100
	sb x2, 0x0(x31)

	// scan arc 16
	addi x2, x0, 16
	sb x2, 0x25(x31)

	// scan range 50
	addi x2, x0, 50
	sh x2, 0x26(x31)

	addi x2, x0, 16
mainloop:
	sh x2, 0x01(x31) // steer hull
	sh x2, 0x12(x31) // steer turret
	sb x1, 0x10(x31) // fire

	// stay straight for a while
	addi x3, x0, 0
	addi x4, x0, 11
delay:
	addi x3, x3, 1
	bge x3, x4, delay

	j mainloop
