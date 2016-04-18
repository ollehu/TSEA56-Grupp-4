void TWISetup(int slaveAddress)
{
	//Set slave address and start TWI (including the TWI-interrupt)
	TWAR = slaveAddress;
	TWCR |= (1<<TWEA)|(1<<TWEN)|(1<<TWIE);
}