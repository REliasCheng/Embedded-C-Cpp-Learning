#include <stdbool.h>
#include <stdio.h>
#include <string.h>

typedef struct {
    char name[20];
    int age;
} Student;

/* Personal practice: every open, transfer and close has a checked result. */
static bool write_records(const char *path, const Student *records, size_t count)
{
    FILE *file = fopen(path, "wb");
    if (file == NULL) {
        return false;
    }
    bool ok = fwrite(records, sizeof(*records), count, file) == count;
    if (fclose(file) != 0) {
        ok = false;
    }
    return ok;
}

static bool read_records(const char *path, Student *records, size_t count)
{
    FILE *file = fopen(path, "rb");
    if (file == NULL) {
        return false;
    }
    bool ok = fread(records, sizeof(*records), count, file) == count;
    if (fclose(file) != 0) {
        ok = false;
    }
    return ok;
}

int main(int argc, char **argv)
{
    if (argc != 2) {
        fprintf(stderr, "usage: checked_records <output-file>\n");
        return 2;
    }

    const Student input[] = {{"Zhang", 18}, {"Li", 28}};
    Student output[sizeof input / sizeof input[0]];
    const size_t count = sizeof input / sizeof input[0];
    memset(output, 0, sizeof output);

    if (!write_records(argv[1], input, count) ||
        !read_records(argv[1], output, count)) {
        fprintf(stderr, "record I/O failed\n");
        return 1;
    }
    for (size_t i = 0; i < count; ++i) {
        if (strcmp(input[i].name, output[i].name) != 0 ||
            input[i].age != output[i].age) {
            fprintf(stderr, "record mismatch\n");
            return 1;
        }
    }
    printf("verified %zu records\n", count);
    return 0;
}
