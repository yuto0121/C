#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MAX 100

void to_uppercase(char *str)
{
    while (*str)
    {
        if (*str >= 'a' && *str <= 'z')
        {
            *str -= 32;
        }
        str++;
    }
}

int is_digit(char ch)
{
    return ch >= '0' && ch <= '9';
}

int is_upper(char ch)
{
    return ch >= 'A' && ch <= 'Z';
}

int is_lower(char ch)
{
    return ch >= 'a' && ch <= 'z';
}

int main()
{
    char first[MAX], second[MAX], name[MAX], str[MAX];
    int year;

    // Read first and second names
    printf("Enter your first name: ");
    scanf("%s", first);
    printf("Enter your second name: ");
    scanf("%s", second);

    // Convert second name to uppercase and store in str
    strcpy(str, second);
    to_uppercase(str);
    printf("Uppercase second name: %s\n", str);

    // Compare second and str
    int cmp_result = strcmp(second, str);
    printf("Comparison result: %d\n", cmp_result);

    // Case-insensitive compare (using strcasecmp)
    cmp_result = strcasecmp(second, str);
    printf("Case-insensitive comparison result: %d\n", cmp_result);

    // Concatenate first and second names
    strcpy(name, first);
    strcat(name, " ");
    strcat(name, second);
    printf("Full name: %s\n", name);

    // Read birth year
    printf("Enter your birth year: ");
    scanf("%d", &year);

    // Concatenate first, second, and year using snprintf
    snprintf(name, MAX, "%s %s %d", first, second, year);
    printf("Full name with year: %s\n", name);

    // Extract first, second, and year using sscanf
    char extracted_first[MAX], extracted_second[MAX];
    int extracted_year;
    sscanf(name, "%s %s %d", extracted_first, extracted_second, &extracted_year);
    printf("Extracted: %s %s %d\n", extracted_first, extracted_second, extracted_year);

    return 0;
}
