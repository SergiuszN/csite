char *number_to_string(const int number) {
    char buf[32];
    sprintf(buf, "%d", number);
    return strdup(buf);
}
