int main(void)
{
    char s2[112];
    char ptr[48];
    char s[100];
    int v7;
    FILE *stream;

    memset(s, 0, sizeof(s));
    memset(ptr, 0, 41);
    memset(s2, 0, 100);
    stream = nullptr;
    v7 = 0;
    stream = fopen("/home/users/level03/.pass", "r");
    if ( stream == nullptr )
    {
        fwrite("ERROR: failed to open password file\n", 1u, 36, stderr);
        exit(1);
    }
    v7 = fread(ptr, 1u, 41, stream);
    ptr[strcspn(ptr, "\n")] = 0;
    if ( v7 != 41 )
    {
        fwrite("ERROR: failed to read password file\n", 1u, 36, stderr);
        fwrite("ERROR: failed to read password file\n", 1u, 36, stderr);
        exit(1);
    }
    fclose(stream);
    puts("===== [ Secure Access System v1.0 ] =====");
    puts("/***************************************\\");
    puts("| You must login to access this system. |");
    puts("\\**************************************/");
    printf("--[ Username: ");
    fgets(s, 100, stdin);
    s[strcspn(s, "\n")] = 0;
    printf("--[ Password: ");
    fgets(s2, 100, stdin);
    s2[strcspn(s2, "\n")] = 0;
    puts("*****************************************");
    if ( strncmp(ptr, s2, 41) != 0 )
    {
        printf(s);
        puts(" does not have access!");
        exit(1);
    }
    printf("Greetings, %s!\n", s);
    system("/bin/sh");
    return 0;
}