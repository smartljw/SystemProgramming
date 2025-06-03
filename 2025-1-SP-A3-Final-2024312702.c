#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define N 30000  // 입력 데이터터 크기
#define REPEAT 25   // 반복 횟수

typedef struct {
    int key;
    char dummy[100];
} Element;

// 배열 
void copy_array(Element* dst, Element* src, int n) {
    int i;
    for (i = 0; i < n; i++) dst[i] = src[i];
}

Element delete_max_array(Element* arr, int n) {
    int i, max_po = 0;
    Element max = arr[0];
    for (i = 1; i < n; i++) {
        if (arr[i].key > max.key) {
            max = arr[i];
            max_po = i;
        }
    }
    arr[max_po].key = -1;
    return max;
}

// 연결 리스트 
typedef struct ListNode {
    Element data;
    struct ListNode* next;
} ListNode;

ListNode* insert_list(ListNode* head, Element item) {
    ListNode* node = (ListNode*)malloc(sizeof(ListNode));
    node->data = item;
    node->next = head;
    return node;
}

Element delete_max_list(ListNode* head) {
    int i;
    ListNode* p = head;
    ListNode* max_node = head;
    Element max = head->data;

    while (p != NULL) {
        if (p->data.key > max.key) {
            max = p->data;
            max_node = p;
        }
        p = p->next;
    }

    if (max_node->next != NULL) {
        ListNode* temp = max_node->next;
        max_node->data = temp->data;
        max_node->next = temp->next;
        free(temp);
    }

    return max;
}

// BST
typedef struct TreeNode {
    Element data;
    struct TreeNode* left;
    struct TreeNode* right;
} TreeNode;

TreeNode* insert_bst(TreeNode* root, Element item) {
    if (!root) {
        TreeNode* node = (TreeNode*)malloc(sizeof(TreeNode));
        node->data = item;
        node->left = node->right = NULL;
        return node;
    }
    if (item.key < root->data.key) {
        root->left = insert_bst(root->left, item);
    } else {
        root->right = insert_bst(root->right, item);
    }

    return root;
}

TreeNode* find_max_bst(TreeNode* node) {
    while (node->right) node = node->right;
    return node;
}

TreeNode* delete_bst(TreeNode* root, int key, Element* out) {
    if (!root) return NULL;

    if (key < root->data.key) {
        root->left = delete_bst(root->left, key, out);
    } else if (key > root->data.key) {
        root->right = delete_bst(root->right, key, out);
    } else {
        *out = root->data;
        if (!root->left) {
            TreeNode* temp = root->right;
            free(root);
            return temp;
        } else if (!root->right) {
            TreeNode* temp = root->left;
            free(root);
            return temp;
        }
        TreeNode* max_node = find_max_bst(root->left);
        root->data = max_node->data;
        root->left = delete_bst(root->left, max_node->data.key, out);
    }

    return root;
}

// merge sort 위한 트리
typedef struct LeftistNode {
    Element data;
    int num;
    struct LeftistNode* left;
    struct LeftistNode* right;
} LeftistNode;

LeftistNode* create_node(Element data) {
    LeftistNode* node = (LeftistNode*)malloc(sizeof(LeftistNode));
    node->data = data;
    node->left = node->right = NULL;
    node->num = 0;

    return node;
}

LeftistNode* merge_heap(LeftistNode* a, LeftistNode* b) {
    LeftistNode* temp;
    if (!a) return b;
    if (!b) return a;

    if (a->data.key < b->data.key) {
        temp = a;
        a = b;
        b = temp;
    }
    a->right = merge_heap(a->right, b);

    if (!a->left || a->left->num < a->right->num) {
        temp = a->left;
        a->left = a->right;
        a->right = temp;
    }
    a->num = (a->right ? a->right->num : -1) + 1;

    return a;
}

LeftistNode* insert_heap(LeftistNode* root, Element data) {
    return merge_heap(root, create_node(data));
}

LeftistNode* delete_max_heap(LeftistNode* root, Element* out) {
    LeftistNode* new_root;

    if (!root) return NULL;

    *out = root->data;
    new_root = merge_heap(root->left, root->right);

    free(root);

    return new_root;
}

// 데이터 생성
unsigned int rand_LCG(unsigned int* seed) {
    *seed = (*seed * 1103515245 + 12345) & 0x7fffffff;
    return *seed;
}

void generate_unique(Element* data, int n) {
    int i;
    srand((unsigned)time(NULL));
    for(i = 0; i < n; i+=2) {
        data[i].key = i+1;
        data[i+1].key = i+2;
    }
    for(; i < n; i++) {
        data[i].key = i+1;
    }

    unsigned int seed = (unsigned int)rand();
    for(i = n-1; i > 0; i--) {
        int j = rand_LCG(&seed) % (i + 1);
        Element temp = data[i];
        data[i] = data[j];
        data[j] = temp;
    }
}

// main
int main() {
    clock_t total_time_start = clock();

    Element* data = (Element*)malloc(sizeof(Element) * N);
    Element* arr = (Element*)malloc(sizeof(Element) * N);
    ListNode* list = NULL;
    TreeNode* bst = NULL;
    LeftistNode* heap = NULL;
    Element temp;
    double time;
    clock_t start, end;
    int i, r;

    printf("실습 데이터 수: %d, 반복 횟수: %d회\n\n", N, REPEAT);
    generate_unique(data, N);

    // 배열
    time = 0;
    for (r = 0; r < REPEAT; r++) {
        copy_array(arr, data, N);
        start = clock();
        for (i = 0; i < N; i++) delete_max_array(arr, N);
        end = clock();
        time += (double)(end - start) / CLOCKS_PER_SEC;
    }
    printf("Array       : %.5f sec\n", time / REPEAT);

    // 연결 리스트
    list = NULL;
    for (i = 0; i < N; i++) list = insert_list(list, data[i]);
    time = 0;
    for (r = 0; r < REPEAT; r++) {
        start = clock();
        for (i = 0; i < N; i++) delete_max_list(list);
        end = clock();
        time += (double)(end - start) / CLOCKS_PER_SEC;
    }
    printf("Linked List : %.5f sec\n", time / REPEAT);

    // BST
    time = 0;
    for (r = 0; r < REPEAT; r++) {
        bst = NULL;
        for (i = 0; i < N; i++) bst = insert_bst(bst, data[i]);
        start = clock();
        for (i = 0; i < N; i++) {
            TreeNode* max_node = find_max_bst(bst);
            bst = delete_bst(bst, max_node->data.key, &temp);
        }
        end = clock();
        time += (double)(end - start) / CLOCKS_PER_SEC;
    }
    printf("BST         : %.5f sec\n", time / REPEAT);

    // 머지소트를 위한 트리
    time = 0;
    for (r = 0; r < REPEAT; r++) {
        heap = NULL;
        for (i = 0; i < N; i++) heap = insert_heap(heap, data[i]);
        start = clock();
        for (i = 0; i < N; i++) heap = delete_max_heap(heap, &temp);
        end = clock();
        time += (double)(end - start) / CLOCKS_PER_SEC;
    }
    printf("LeftistHeap : %.5f sec\n", time / REPEAT);

    free(data);
    free(arr);

    printf("\n");
    clock_t total_time_end = clock();
    double total_time = (double)(total_time_end - total_time_start) / CLOCKS_PER_SEC;
    printf("전체 코드 실행 시간: %.3f sec\n", total_time);

    return 0;
}
