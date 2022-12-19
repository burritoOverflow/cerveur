#include "Routes.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void traverseRoutes(struct Route *root) {
  if (root != NULL) {
    traverseRoutes(root->left);
    printf("%s -> %s \n", root->key, root->value);
    traverseRoutes(root->right);
  }
}

struct Route *addRoute(struct Route *root, char *key, char *value) {
  if (root == NULL) {
    struct Route *route = (struct Route *)malloc(sizeof(struct Route));
    route->key = key;
    route->value = value;
    route->left = route->right = NULL;
    return route;
  } else {
    if (strcmp(key, root->key) == 0) {
      printf("WARNING: A Route For \"%s\" Already Exists\n", key);
      return root;
    } else if (strcmp(key, root->key) > 0) {
      return root->right = addRoute(root->right, key, value);
    } else { // where (strcmp(key, root->key) < 0)
      return root->left = addRoute(root->left, key, value);
    }
  }
}

struct Route *search(struct Route *root, char *key) {
  if (root == NULL) {
    return NULL;
  } else {
    if (strcmp(key, root->key) == 0) {
      return root;
    } else if (strcmp(key, root->key) > 0) {
      return search(root->right, key);
      // Where (strcmp(key, root->key) < 0
    } else {
      return search(root->left, key);
    }
  }
}
