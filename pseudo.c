

// ordenan los 32

// barrera 32

if (id % 4 = 0)
{

    mergeas tu parte con tu parte

    // barrera 16
}

if (id % 8 = 0)
{

    mergeas tu parte con tu parte * 4

    // barrera 8
}

if (id % 16 = 0)
{
    mergeas tu parte con tu parte * 8
    // barrera 4
}

if (id % 32 = 0)
{
    mergeas tu parte con tu parte * 16
    // barrera 2
}

if (id % 64 = 0)
{
    mergeas tu parte con tu parte * 62
}

// Una vez que todos terminan de ordenas su parte
for (i = 4; i <= THREADS; i *= 2)
{

    if (id % i == 0)
    {
        // rangoQueProcesas = rangoQueProcesas*2;
        // Mergeas el rango que te toca procesar (Las dos mitades)
        // barreraX  --> Barrera[i] la esperan 64/i procesos, por ejemplo cuando i=4, la barrera es de 16, cuando i es de 8 la barrera es de 8...
    }
}