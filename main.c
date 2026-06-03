#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <gumbo.h>

struct MemoryStruct {
    char *memory;
    size_t size;
};

typedef struct {
    char **items;
    size_t count;
    size_t capacity;
} UniqueList;

void init_list(UniqueList *list) {
    list->count = 0;
    list->capacity = 16;
    list->items = malloc(list->capacity * sizeof(char *));
}

void add_to_list(UniqueList *list, const char *item) {
    if (!item || strlen(item) == 0) return;

    for (size_t i = 0; i < list->count; i++) {
        if (strcmp(list->items[i], item) == 0) {
            return;
        }
    }

    if (list->count >= list->capacity) {
        list->capacity *= 2;
        char **new_items = realloc(list->items, list->capacity * sizeof(char *));
        if (!new_items) {
            exit(EXIT_FAILURE);
        }
        list->items = new_items;
    }

    list->items[list->count] = strdup(item);
    list->count++;
}

void free_list(UniqueList *list) {
    for (size_t i = 0; i < list->count; i++) {
        free(list->items[i]);
    }
    free(list->items);
}

static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;
    struct MemoryStruct *mem = (struct MemoryStruct *)userp;

    char *ptr = realloc(mem->memory, mem->size + realsize + 1);
    if (!ptr) {
        return 0;
    }

    mem->memory = ptr;
    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;

    return realsize;
}

void parse_dom_tree(GumboNode* node, UniqueList *links, UniqueList *images) {
    if (node->type != GUMBO_NODE_ELEMENT) return;

    GumboAttribute* href;
    GumboAttribute* src;

    if (node->v.element.tag == GUMBO_TAG_A &&
        (href = gumbo_get_attribute(&node->v.element.attributes, "href"))) {
        add_to_list(links, href->value);
    }
    else if (node->v.element.tag == GUMBO_TAG_IMG &&
             (src = gumbo_get_attribute(&node->v.element.attributes, "src"))) {
        add_to_list(images, src->value);
    }

    GumboVector* children = &node->v.element.children;
    for (unsigned int i = 0; i < children->length; ++i) {
        parse_dom_tree((GumboNode*)children->data[i], links, images);
    }
}

void write_index_file(const char *filename, const char *target_url, UniqueList *links, UniqueList *images) {
    FILE *fp = fopen(filename, "w");
    if (!fp) return;

    fprintf(fp, "{\n");
    fprintf(fp, "    \"target_url\": \"%s\",\n", target_url);
    fprintf(fp, "    \"total_links\": %zu,\n", links->count);
    fprintf(fp, "    \"total_images\": %zu,\n", images->count);
    
    fprintf(fp, "    \"links\": [\n");
    for (size_t i = 0; i < links->count; i++) {
        fprintf(fp, "        \"%s\"%s\n", links->items[i], (i + 1 < links->count) ? "," : "");
    }
    fprintf(fp, "    ],\n");

    fprintf(fp, "    \"images\": [\n");
    for (size_t i = 0; i < images->count; i++) {
        fprintf(fp, "        \"%s\"%s\n", images->items[i], (i + 1 < images->count) ? "," : "");
    }
    fprintf(fp, "    ]\n");
    fprintf(fp, "}\n");

    fclose(fp);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        return EXIT_FAILURE;
    }

    CURL *curl_handle;
    CURLcode res;
    struct MemoryStruct chunk = { .memory = malloc(1), .size = 0 };

    UniqueList links_list;
    UniqueList images_list;
    init_list(&links_list);
    init_list(&images_list);

    curl_global_init(CURL_GLOBAL_ALL);
    curl_handle = curl_easy_init();

    curl_easy_setopt(curl_handle, CURLOPT_URL, argv[1]);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);
    curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");

    res = curl_easy_perform(curl_handle);

    if (res == CURLE_OK) {
        GumboOutput* output = gumbo_parse(chunk.memory);
        parse_dom_tree(output->root, &links_list, &images_list);
        write_index_file("index.json", argv[1], &links_list, &images_list);
        gumbo_destroy_output(&kGumboDefaultOptions, output);
    }

    curl_easy_cleanup(curl_handle);
    free(chunk.memory);
    free_list(&links_list);
    free_list(&images_list);
    curl_global_cleanup();

    return EXIT_SUCCESS;
}
