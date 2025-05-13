#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#define MAX_EVENTS 500  // Increased maximum events
#define KEY_SIZE 32     // Stronger encryption key size
#define DESCRIPTION_SIZE 200  // Larger description field

// Improved encryption key
const char ENCRYPTION_KEY[KEY_SIZE] = "9f42cb71de86a0e415ad563ef28029ba";

typedef struct {
    int id;
    int day, month, year;
    int hour, minute;
    char description[DESCRIPTION_SIZE];
    int priority;  // New: 1-5 priority level
    char category[50];  // New: category field
} Event;

typedef struct {
    Event events[MAX_EVENTS];
    int event_count;
    int next_id;  // For unique ID assignment
} Schedule;

Schedule schedule = {.event_count = 0, .next_id = 1};

// Function prototypes
void clear_input_buffer();
int validate_date(int day, int month, int year);
int validate_time(int hour, int minute);
void xor_encrypt_decrypt(char *data, const char *key, size_t data_len);
void add_event();
void view_schedule();
void view_today_events();
void sort_events();
void delete_event();
void save_schedule();
void load_schedule();
void search_events();
void edit_event();
void print_event(Event e, int index);
void export_to_text();
void show_statistics();
void help();

int main() {
    int choice = 0;

    // Try to load existing schedule on startup
    load_schedule();

    while (1) {
        printf("\n===== SCHEDULE MANAGER =====\n");
        printf("1. Add Event\n");
        printf("2. View All Events\n");
        printf("3. View Today's Events\n");
        printf("4. Search Events\n");
        printf("5. Edit Event\n");
        printf("6. Delete Event\n");
        printf("7. Sort Events\n");
        printf("8. Save Schedule\n");
        printf("9. Export to Text File\n");
        printf("10. Show Statistics\n");
        printf("11. Help\n");
        printf("0. Exit\n");
        printf("Choice: ");

        if (scanf("%d", &choice) != 1) {
            printf("Invalid input. Please enter a number.\n");
            clear_input_buffer();
            continue;
        }

        switch (choice) {
            case 0:
                printf("Saving schedule before exit...\n");
                save_schedule();
                printf("Goodbye!\n");
                return 0;
            case 1:
                add_event();
                break;
            case 2:
                view_schedule();
                break;
            case 3:
                view_today_events();
                break;
            case 4:
                search_events();
                break;
            case 5:
                edit_event();
                break;
            case 6:
                delete_event();
                break;
            case 7:
                sort_events();
                break;
            case 8:
                save_schedule();
                break;
            case 9:
                export_to_text();
                break;
            case 10:
                show_statistics();
                break;
            case 11:
                help();
                break;
            default:
                printf("Invalid choice. Please try again.\n");
        }
    }

    return 0;
}

void clear_input_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// Date validation function
int validate_date(int day, int month, int year) {
    if (year < 2000 || year > 2100) return 0;
    if (month < 1 || month > 12) return 0;

    int days_in_month[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    // Adjust for leap years
    if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)) {
        days_in_month[2] = 29;
    }

    return (day >= 1 && day <= days_in_month[month]);
}

// Time validation function
int validate_time(int hour, int minute) {
    return (hour >= 0 && hour <= 23 && minute >= 0 && minute <= 59);
}

// Improved encryption function
void xor_encrypt_decrypt(char *data, const char *key, size_t data_len) {
    size_t key_len = strlen(key);
    for (size_t i = 0; i < data_len; i++) {
        data[i] ^= key[i % key_len];
    }
}

void add_event() {
    if (schedule.event_count >= MAX_EVENTS) {
        printf("Event list full! Please delete some events first.\n");
        return;
    }

    Event e;
    e.id = schedule.next_id++;

    int valid_date = 0;
    while (!valid_date) {
        printf("Enter date (DD MM YYYY): ");
        if (scanf("%d %d %d", &e.day, &e.month, &e.year) != 3) {
            printf("Invalid input format. Please use DD MM YYYY.\n");
            clear_input_buffer();
            continue;
        }

        if (validate_date(e.day, e.month, e.year)) {
            valid_date = 1;
        } else {
            printf("Invalid date. Please try again.\n");
            clear_input_buffer();
        }
    }

    int valid_time = 0;
    while (!valid_time) {
        printf("Enter time (HH MM): ");
        if (scanf("%d %d", &e.hour, &e.minute) != 2) {
            printf("Invalid input format. Please use HH MM.\n");
            clear_input_buffer();
            continue;
        }

        if (validate_time(e.hour, e.minute)) {
            valid_time = 1;
        } else {
            printf("Invalid time. Please try again.\n");
        }
    }

    printf("Enter description: ");
    clear_input_buffer();
    fgets(e.description, DESCRIPTION_SIZE, stdin);
    e.description[strcspn(e.description, "\n")] = 0; // remove newline

    int valid_priority = 0;
    while (!valid_priority) {
        printf("Enter priority (1-5, 1=highest): ");
        if (scanf("%d", &e.priority) != 1) {
            printf("Invalid input. Please enter a number.\n");
            clear_input_buffer();
            continue;
        }

        if (e.priority >= 1 && e.priority <= 5) {
            valid_priority = 1;
        } else {
            printf("Priority must be between 1 and 5.\n");
        }
    }

    printf("Enter category: ");
    clear_input_buffer();
    fgets(e.category, 50, stdin);
    e.category[strcspn(e.category, "\n")] = 0; // remove newline

    schedule.events[schedule.event_count++] = e;
    printf("Event added successfully with ID: %d\n", e.id);
}

void print_event(Event e, int index) {
    char priority_indicator[6] = "     ";
    for (int i = 0; i < e.priority; i++) {
        priority_indicator[i] = '*';
    }

    printf("#%d [ID: %d] %02d/%02d/%04d %02d:%02d %s - %s [%s]\n",
           index, e.id, e.day, e.month, e.year,
           e.hour, e.minute, priority_indicator,
           e.description, e.category);
}

void view_schedule() {
    if (schedule.event_count == 0) {
        printf("No events scheduled.\n");
        return;
    }

    printf("\n===== ALL EVENTS =====\n");
    for (int i = 0; i < schedule.event_count; i++) {
        print_event(schedule.events[i], i);
    }
}

void view_today_events() {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    int today_day = t->tm_mday;
    int today_month = t->tm_mon + 1;
    int today_year = t->tm_year + 1900;

    printf("\n===== TODAY'S EVENTS (%02d/%02d/%04d) =====\n",
           today_day, today_month, today_year);

    int found = 0;
    for (int i = 0; i < schedule.event_count; i++) {
        if (schedule.events[i].day == today_day &&
            schedule.events[i].month == today_month &&
            schedule.events[i].year == today_year) {
            print_event(schedule.events[i], i);
            found++;
        }
    }

    if (!found) {
        printf("No events scheduled for today.\n");
    }
}

// Compare function for sorting events by date and time
int compare_events(const void *a, const void *b) {
    Event *e1 = (Event *)a;
    Event *e2 = (Event *)b;

    if (e1->year != e2->year) return e1->year - e2->year;
    if (e1->month != e2->month) return e1->month - e2->month;
    if (e1->day != e2->day) return e1->day - e2->day;
    if (e1->hour != e2->hour) return e1->hour - e2->hour;
    return e1->minute - e2->minute;
}

// Compare function for sorting events by priority
int compare_events_priority(const void *a, const void *b) {
    Event *e1 = (Event *)a;
    Event *e2 = (Event *)b;
    return e1->priority - e2->priority;
}

void sort_events() {
    if (schedule.event_count == 0) {
        printf("No events to sort.\n");
        return;
    }

    int sort_choice = 0;
    printf("Sort by:\n");
    printf("1. Date and Time\n");
    printf("2. Priority\n");
    printf("Choice: ");

    if (scanf("%d", &sort_choice) != 1) {
        printf("Invalid input.\n");
        clear_input_buffer();
        return;
    }

    switch (sort_choice) {
        case 1:
            qsort(schedule.events, schedule.event_count, sizeof(Event), compare_events);
            printf("Events sorted by date and time.\n");
            break;
        case 2:
            qsort(schedule.events, schedule.event_count, sizeof(Event), compare_events_priority);
            printf("Events sorted by priority.\n");
            break;
        default:
            printf("Invalid choice.\n");
    }
}

void delete_event() {
    if (schedule.event_count == 0) {
        printf("No events to delete.\n");
        return;
    }

    view_schedule();

    int id_to_delete;
    printf("Enter event ID to delete: ");
    if (scanf("%d", &id_to_delete) != 1) {
        printf("Invalid input.\n");
        clear_input_buffer();
        return;
    }

    int found = 0;
    for (int i = 0; i < schedule.event_count; i++) {
        if (schedule.events[i].id == id_to_delete) {
            printf("Deleting event: ");
            print_event(schedule.events[i], i);

            // Shift all events down to fill the gap
            for (int j = i; j < schedule.event_count - 1; j++) {
                schedule.events[j] = schedule.events[j + 1];
            }
            schedule.event_count--;
            found = 1;
            break;
        }
    }

    if (found) {
        printf("Event deleted successfully.\n");
    } else {
        printf("Event ID not found.\n");
    }
}

void save_schedule() {
    FILE *fp = fopen("schedule.dat", "wb");
    if (!fp) {
        printf("Error opening file for writing.\n");
        return;
    }

    // First write the event count and next ID
    fprintf(fp, "%d %d\n", schedule.event_count, schedule.next_id);

    // Then write each event
    for (int i = 0; i < schedule.event_count; i++) {
        Event e = schedule.events[i];

        // Format the data as a string
        char buffer[512];
        sprintf(buffer, "%d|%d|%d|%d|%d|%d|%d|%s|%s\n",
                e.id, e.day, e.month, e.year, e.hour, e.minute,
                e.priority, e.category, e.description);

        // Encrypt the buffer
        xor_encrypt_decrypt(buffer, ENCRYPTION_KEY, strlen(buffer));

        // Write to file
        fwrite(buffer, sizeof(char), strlen(buffer), fp);
    }

    fclose(fp);
    printf("Schedule saved successfully.\n");
}

void load_schedule() {
    FILE *fp = fopen("schedule.dat", "rb");
    if (!fp) {
        printf("No existing schedule file found.\n");
        return;
    }

    // First read the event count and next ID
    if (fscanf(fp, "%d %d\n", &schedule.event_count, &schedule.next_id) != 2) {
        printf("Error reading schedule metadata.\n");
        fclose(fp);
        return;
    }

    // Check if event count is valid
    if (schedule.event_count > MAX_EVENTS) {
        printf("Error: File contains too many events.\n");
        fclose(fp);
        return;
    }

    // Buffer for reading encrypted lines
    char buffer[512];
    int event_index = 0;

    // Read each event
    while (event_index < schedule.event_count && fgets(buffer, sizeof(buffer), fp)) {
        // Decrypt the buffer
        xor_encrypt_decrypt(buffer, ENCRYPTION_KEY, strlen(buffer));

        Event *e = &schedule.events[event_index];

        // Parse the event data with safer parsing
        char description_buffer[DESCRIPTION_SIZE] = {0};
        char category_buffer[50] = {0};

        if (sscanf(buffer, "%d|%d|%d|%d|%d|%d|%d|%49[^|]|%199[^\n]",
                  &e->id, &e->day, &e->month, &e->year,
                  &e->hour, &e->minute, &e->priority,
                  category_buffer, description_buffer) == 9) {

            strncpy(e->category, category_buffer, 49);
            strncpy(e->description, description_buffer, DESCRIPTION_SIZE - 1);
            e->category[49] = '\0';
            e->description[DESCRIPTION_SIZE - 1] = '\0';

            event_index++;
        } else {
            printf("Warning: Skipped invalid event record.\n");
        }
    }

    // Update event count in case some events were invalid
    schedule.event_count = event_index;

    fclose(fp);
    printf("Schedule loaded successfully. %d events found.\n", schedule.event_count);
}

void search_events() {
    if (schedule.event_count == 0) {
        printf("No events to search.\n");
        return;
    }

    int search_choice;
    printf("\n===== SEARCH OPTIONS =====\n");
    printf("1. Search by keyword\n");
    printf("2. Search by date\n");
    printf("3. Search by category\n");
    printf("Choice: ");

    if (scanf("%d", &search_choice) != 1) {
        printf("Invalid input.\n");
        clear_input_buffer();
        return;
    }

    switch (search_choice) {
        case 1: {
            char keyword[100];
            printf("Enter keyword to search: ");
            clear_input_buffer();
            fgets(keyword, 100, stdin);
            keyword[strcspn(keyword, "\n")] = 0;

            // Convert keyword to lowercase for case-insensitive search
            for (int i = 0; keyword[i]; i++) {
                keyword[i] = tolower(keyword[i]);
            }

            printf("\n===== SEARCH RESULTS =====\n");
            int found = 0;

            for (int i = 0; i < schedule.event_count; i++) {
                // Make lowercase copies for case-insensitive comparison
                char desc_lower[DESCRIPTION_SIZE];
                char cat_lower[50];

                strncpy(desc_lower, schedule.events[i].description, DESCRIPTION_SIZE);
                strncpy(cat_lower, schedule.events[i].category, 50);

                for (int j = 0; desc_lower[j]; j++) {
                    desc_lower[j] = tolower(desc_lower[j]);
                }

                for (int j = 0; cat_lower[j]; j++) {
                    cat_lower[j] = tolower(cat_lower[j]);
                }

                if (strstr(desc_lower, keyword) || strstr(cat_lower, keyword)) {
                    print_event(schedule.events[i], i);
                    found++;
                }
            }

            if (!found) {
                printf("No matching events found.\n");
            } else {
                printf("Found %d matching events.\n", found);
            }
            break;
        }
        case 2: {
            int day, month, year;
            printf("Enter date to search (DD MM YYYY): ");
            if (scanf("%d %d %d", &day, &month, &year) != 3) {
                printf("Invalid input format.\n");
                clear_input_buffer();
                return;
            }

            if (!validate_date(day, month, year)) {
                printf("Invalid date.\n");
                return;
            }

            printf("\n===== EVENTS ON %02d/%02d/%04d =====\n", day, month, year);
            int found = 0;

            for (int i = 0; i < schedule.event_count; i++) {
                if (schedule.events[i].day == day &&
                    schedule.events[i].month == month &&
                    schedule.events[i].year == year) {
                    print_event(schedule.events[i], i);
                    found++;
                }
            }

            if (!found) {
                printf("No events found on this date.\n");
            }
            break;
        }
        case 3: {
            char category[50];
            printf("Enter category to search: ");
            clear_input_buffer();
            fgets(category, 50, stdin);
            category[strcspn(category, "\n")] = 0;

            // Convert category to lowercase for case-insensitive search
            for (int i = 0; category[i]; i++) {
                category[i] = tolower(category[i]);
            }

            printf("\n===== EVENTS IN CATEGORY =====\n");
            int found = 0;

            for (int i = 0; i < schedule.event_count; i++) {
                char cat_lower[50];
                strncpy(cat_lower, schedule.events[i].category, 50);

                for (int j = 0; cat_lower[j]; j++) {
                    cat_lower[j] = tolower(cat_lower[j]);
                }

                if (strstr(cat_lower, category)) {
                    print_event(schedule.events[i], i);
                    found++;
                }
            }

            if (!found) {
                printf("No events found in this category.\n");
            }
            break;
        }
        default:
            printf("Invalid choice.\n");
    }
}

void edit_event() {
    if (schedule.event_count == 0) {
        printf("No events to edit.\n");
        return;
    }

    view_schedule();

    int id_to_edit;
    printf("Enter event ID to edit: ");
    if (scanf("%d", &id_to_edit) != 1) {
        printf("Invalid input.\n");
        clear_input_buffer();
        return;
    }

    int found = 0;
    int index = 0;

    for (int i = 0; i < schedule.event_count; i++) {
        if (schedule.events[i].id == id_to_edit) {
            found = 1;
            index = i;
            break;
        }
    }

    if (!found) {
        printf("Event ID not found.\n");
        return;
    }

    Event *e = &schedule.events[index];
    printf("Editing event: ");
    print_event(*e, index);

    int edit_choice;
    printf("\n===== EDIT OPTIONS =====\n");
    printf("1. Edit date\n");
    printf("2. Edit time\n");
    printf("3. Edit description\n");
    printf("4. Edit priority\n");
    printf("5. Edit category\n");
    printf("0. Cancel\n");
    printf("Choice: ");

    if (scanf("%d", &edit_choice) != 1) {
        printf("Invalid input.\n");
        clear_input_buffer();
        return;
    }

    switch (edit_choice) {
        case 0:
            printf("Edit cancelled.\n");
            return;
        case 1: {
            int day, month, year;
            printf("Enter new date (DD MM YYYY): ");
            if (scanf("%d %d %d", &day, &month, &year) != 3) {
                printf("Invalid input format.\n");
                clear_input_buffer();
                return;
            }

            if (validate_date(day, month, year)) {
                e->day = day;
                e->month = month;
                e->year = year;
                printf("Date updated.\n");
            } else {
                printf("Invalid date. No changes made.\n");
            }
            break;
        }
        case 2: {
            int hour, minute;
            printf("Enter new time (HH MM): ");
            if (scanf("%d %d", &hour, &minute) != 2) {
                printf("Invalid input format.\n");
                clear_input_buffer();
                return;
            }

            if (validate_time(hour, minute)) {
                e->hour = hour;
                e->minute = minute;
                printf("Time updated.\n");
            } else {
                printf("Invalid time. No changes made.\n");
            }
            break;
        }
        case 3: {
            printf("Enter new description: ");
            clear_input_buffer();
            fgets(e->description, DESCRIPTION_SIZE, stdin);
            e->description[strcspn(e->description, "\n")] = 0;
            printf("Description updated.\n");
            break;
        }
        case 4: {
            int priority;
            printf("Enter new priority (1-5, 1=highest): ");
            if (scanf("%d", &priority) != 1) {
                printf("Invalid input.\n");
                clear_input_buffer();
                return;
            }

            if (priority >= 1 && priority <= 5) {
                e->priority = priority;
                printf("Priority updated.\n");
            } else {
                printf("Invalid priority. No changes made.\n");
            }
            break;
        }
        case 5: {
            printf("Enter new category: ");
            clear_input_buffer();
            fgets(e->category, 50, stdin);
            e->category[strcspn(e->category, "\n")] = 0;
            printf("Category updated.\n");
            break;
        }
        default:
            printf("Invalid choice.\n");
    }
}

void export_to_text() {
    if (schedule.event_count == 0) {
        printf("No events to export.\n");
        return;
    }

    char filename[100];
    printf("Enter filename for export (e.g., schedule.txt): ");
    clear_input_buffer();
    fgets(filename, 100, stdin);
    filename[strcspn(filename, "\n")] = 0;

    FILE *fp = fopen(filename, "w");
    if (!fp) {
        printf("Error opening file for writing.\n");
        return;
    }

    fprintf(fp, "===== SCHEDULE EXPORT =====\n");
    fprintf(fp, "Generated on: ");

    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    fprintf(fp, "%02d/%02d/%04d %02d:%02d\n\n",
           t->tm_mday, t->tm_mon + 1, t->tm_year + 1900,
           t->tm_hour, t->tm_min);

    // Sort events by date before exporting
    qsort(schedule.events, schedule.event_count, sizeof(Event), compare_events);

    for (int i = 0; i < schedule.event_count; i++) {
        Event e = schedule.events[i];

        char priority_indicator[6] = "     ";
        for (int j = 0; j < e.priority; j++) {
            priority_indicator[j] = '*';
        }

        fprintf(fp, "Event #%d [ID: %d]\n", i+1, e.id);
        fprintf(fp, "Date: %02d/%02d/%04d\n", e.day, e.month, e.year);
        fprintf(fp, "Time: %02d:%02d\n", e.hour, e.minute);
        fprintf(fp, "Priority: %s (%d/5)\n", priority_indicator, e.priority);
        fprintf(fp, "Category: %s\n", e.category);
        fprintf(fp, "Description: %s\n\n", e.description);
    }

    fclose(fp);
    printf("Schedule exported to %s successfully.\n", filename);
}

void show_statistics() {
    if (schedule.event_count == 0) {
        printf("No events to analyze.\n");
        return;
    }

    printf("\n===== SCHEDULE STATISTICS =====\n");
    printf("Total events: %d\n", schedule.event_count);

    // Get current date
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    int today_day = t->tm_mday;
    int today_month = t->tm_mon + 1;
    int today_year = t->tm_year + 1900;

    // Count events by priority
    int priority_counts[5] = {0};

    // Count events today
    int events_today = 0;

    // Count events this month
    int events_this_month = 0;

    // Count unique categories
    char categories[MAX_EVENTS][50];
    int category_count = 0;

    for (int i = 0; i < schedule.event_count; i++) {
        Event e = schedule.events[i];

        // Count by priority
        priority_counts[e.priority - 1]++;

        // Check if event is today
        if (e.day == today_day && e.month == today_month && e.year == today_year) {
            events_today++;
        }

        // Check if event is this month
        if (e.month == today_month && e.year == today_year) {
            events_this_month++;
        }

        // Check if category is unique
        int found = 0;
        for (int j = 0; j < category_count; j++) {
            if (strcmp(e.category, categories[j]) == 0) {
                found = 1;
                break;
            }
        }

        if (!found && e.category[0] != '\0') {
            strncpy(categories[category_count], e.category, 50);
            category_count++;
        }
    }

    printf("Events today: %d\n", events_today);
    printf("Events this month: %d\n", events_this_month);
    printf("Unique categories: %d\n\n", category_count);

    printf("Priority distribution:\n");
    for (int i = 0; i < 5; i++) {
        printf("Priority %d: %d events (%.1f%%)\n",
               i+1, priority_counts[i],
               (float)priority_counts[i] / schedule.event_count * 100);
    }

    // Find next event
    if (schedule.event_count > 0) {
        // Sort events first
        qsort(schedule.events, schedule.event_count, sizeof(Event), compare_events);

        // Find next event from today
        for (int i = 0; i < schedule.event_count; i++) {
            Event e = schedule.events[i];

            // Check if event is today or in the future
            if ((e.year > today_year) ||
                (e.year == today_year && e.month > today_month) ||
                (e.year == today_year && e.month == today_month && e.day >= today_day)) {

                printf("\nNext upcoming event:\n");
                print_event(e, i);
                break;
            }
        }
    }
}

void help() {
    printf("\n===== HELP =====\n");
    printf("This schedule manager allows you to manage your events and appointments.\n\n");

    printf("MAIN FEATURES:\n");
    printf("1. Add Event - Create a new event with date, time, description, priority and category\n");
    printf("2. View Events - Display all scheduled events\n");
    printf("3. View Today's Events - Show only events scheduled for today\n");
    printf("4. Search Events - Find events by keyword, date or category\n");
    printf("5. Edit Event - Modify an existing event's details\n");
    printf("6. Delete Event - Remove an event from the schedule\n");
    printf("7. Sort Events - Organize events by date/time or priority\n");
    printf("8. Save Schedule - Store your events to a file\n");
    printf("9. Export to Text - Create a readable text file of your schedule\n");
    printf("10. Show Statistics - Display information about your events\n");
    printf("11. Help - Show this help information\n");
}
