#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

typedef struct node node_t;

struct node {
	uint64_t count;
	node_t **children;
};

typedef struct dev_node {
	node_t base;
	uint16_t code;
	char str[];
} dev_node_t;

typedef struct class_node {
	node_t base;
	uint8_t code; /**< Hex code of this class*/
	char str[]; /**< Name of this class*/
} class_node_t;

void insert_node(node_t *node, node_t *new) {
	node_t **children = realloc(node->children, sizeof(node_t *) * (node->count + 1));

	children[node->count] = new;
	node->count++;
	node->children = children;
}

void print_class_nodes(node_t *root) {
	for(uint64_t i = 0; i < root->count; i++) {
		class_node_t *class = (class_node_t*)root->children[i];
		printf("┏ Class:\n┣┅ Code: %2x\n┣┅ Name: %s\n", class->code, class->str);
		
		for (uint64_t j = 0; j < class->base.count; j++) {
			class_node_t *subclass = (class_node_t*)class->base.children[j];
			printf("┣━━┳ Subclass:\n┃  ┣┅ Code: %2x\n┃  ┣┅ Name: %s\n", subclass->code, subclass->str);
			for (uint64_t k = 0; k < subclass->base.count; k++) {
				class_node_t *pi = (class_node_t*)subclass->base.children[k];
				printf("┃  ┣━━┳ Programming Interface:\n┃  ┃  ┣┅ Code: %2x\n┃  ┃  ┣┅ Name: %s\n", pi->code, pi->str);
				printf("┃  ┃  ┗\n");
			}
			/*PF Here*/
			printf("┃  ┗\n");
		}
		printf("┗\n");
	}
}

void output_generate_headers(FILE *src, FILE *hdr) {
	fprintf(src, "#include <stdint.h>\n\n");

	fprintf(hdr, "#pragma once\n#include <stdint.h>\n\n");
}

void output_generate_vendor_func(node_t *root, FILE *src, FILE *hdr) {

	fprintf(hdr, "const char *pci_vendor_to_str(uint16_t vendor);\n");
	fprintf(src, "const char *pci_vendor_to_str(uint16_t vendor) {\n");
	fprintf(src, "\tswitch(vendor) {\n");
	if(root) {
		for(uint64_t i = 0; i < root->count; i++) {
			dev_node_t *dev = (dev_node_t*)root->children[i];
			fprintf(src, "\t\tcase 0x%.4x: return \"%s\";\n", dev->code, dev->str);
		}
	}

	fprintf(src, "\t\tdefault: return \"Unknown\";\n\t}\n}\n\n");
}

void output_generate_device_func(node_t *root, FILE *src, FILE *hdr) {

	fprintf(hdr, "const char *pci_device_to_str(uint16_t vendor, uint16_t device);\n");
	fprintf(src, "const char *pci_device_to_str(uint16_t vendor, uint16_t device) {\n");
	fprintf(src, "\tswitch(vendor) {\n");

	if(root) {
		for(uint64_t i = 0; i < root->count; i++) {
			dev_node_t *vendor = (dev_node_t*)root->children[i];
			fprintf(src, "\t\tcase 0x%.4x:\n\t\t\tswitch(device) {\n", vendor->code);
			for(uint64_t j = 0; j < vendor->base.count; j++) {
				dev_node_t *device = (dev_node_t*)vendor->base.children[j];
				fprintf(src, "\t\t\t\tcase 0x%.4x: return \"%s\";\n", device->code, device->str);
			}
			fprintf(src, "\t\t\t\tdefault: return \"Unknown\";\n\t\t\t}\n");
		}
	}

	fprintf(src, "\t\tdefault: return \"Unknown\";\n\t}\n}\n\n");
}

void output_generate_class_func(node_t *root, FILE *src, FILE *hdr) {
	
	fprintf(hdr, "const char *pci_class_to_str(uint8_t class);\n");
	fprintf(src, "const char *pci_class_to_str(uint8_t class) {\n");
	fprintf(src, "\tswitch(class) {\n");

	for(uint64_t i = 0; i < root->count; i++) {
		class_node_t *class = (class_node_t*)root->children[i];
		fprintf(src, "\t\tcase 0x%.2x: return \"%s\";\n", class->code, class->str);
	}

	fprintf(src, "\t\tdefault: return \"Unknown\";\n\t}\n}\n\n");
}

void output_generate_subclass_func(node_t *root, FILE *src, FILE *hdr) {
	fprintf(hdr, "const char *pci_subclass_to_str(uint8_t class, uint8_t subclass);\n");
	
	fprintf(src, "const char *pci_subclass_to_str(uint8_t class, uint8_t subclass) {\n");	
	fprintf(src, "\tswitch(class) {\n");

	for(uint64_t i = 0; i < root->count; i++) {
		class_node_t *class = (class_node_t*)root->children[i];
		fprintf(src, "\t\tcase 0x%.2x: {\n", class->code);
		fprintf(src, "\t\t\tswitch(subclass) {\n");

		for (uint64_t j = 0; j < class->base.count; j++) {
			class_node_t *subclass = (class_node_t*)class->base.children[j];
			fprintf(src,"\t\t\t\tcase 0x%.2x: return \"%s\";\n", subclass->code, subclass->str);
		}
		fprintf(src, "\t\t\t\tdefault: return \"Unknown\";\n\t\t\t}\n\t\t}\n");
	}

	fprintf(src, "\t\tdefault: return \"Unknown\";\n\t}\n}\n\n");
}

void output_generate_progif_func(node_t *root, FILE *src, FILE *hdr) {
	fprintf(hdr, "const char *pci_prog_if_to_str(uint8_t class, uint8_t subclass, uint8_t prog_if);\n");

	fprintf(src, "const char *pci_prog_if_to_str(uint8_t class, uint8_t subclass, uint8_t prog_if) {\n");
	fprintf(src, "\tswitch(class) {\n");

	for(uint64_t i = 0; i < root->count; i++) {
		class_node_t *class = (class_node_t*)root->children[i];
		fprintf(src, "\t\tcase 0x%.2x: {\n", class->code);
		fprintf(src, "\t\t\tswitch(subclass) {\n");

		for (uint64_t j = 0; j < class->base.count; j++) {
			class_node_t *subclass = (class_node_t*)class->base.children[j];
			fprintf(src,"\t\t\t\tcase 0x%.2x: {\n", subclass->code);
			fprintf(src, "\t\t\t\t\tswitch(prog_if) {\n");
			for (uint64_t k = 0; k < subclass->base.count; k++) {
				class_node_t *pi = (class_node_t*)subclass->base.children[k];
				fprintf(src, "\t\t\t\t\t\tcase 0x%.2x: return \"%s\";\n", pi->code, pi->str);
			}

			fprintf(src, "\t\t\t\t\t\tdefault: return \"Unknown\";\n\t\t\t\t\t}\n\t\t\t\t}\n");
		}
		fprintf(src, "\t\t\t\tdefault: return \"Unknown\";\n\t\t\t}\n\t\t}\n");
	}

	fprintf(src, "\t\tdefault: return \"Unknown\";\n\t}\n}\n\n");
}

void print_dev_nodes(dev_node_t *root) {
	for(uint64_t i = 0; i < root->base.count; i++) {
		dev_node_t *vendor = (void*)root->base.children[i];
		printf("╔ Vendor:\n╠╍ Hex: %.4x\n╠╍ Name: %s\n", vendor->code, vendor->str);
			for(uint64_t j = 0; j < vendor->base.count; j++) {
				dev_node_t *device = (void*)vendor->base.children[j];
				printf("┣━━┳ Device:\n┃  ┣┅ Hex: %.4x\n┃  ┣┅ Name: %s\n", device->code, device->str);

			}
		printf("┗\n");
	}
}

int is_hex(char ch) {
	if(ch >= '0' && ch <= '9') return 1;
	else if(ch >= 'a' && ch <= 'f') return 1;
	else if(ch >= 'A' && ch <= 'F') return 1;
	return 0;
}


int main(int argc, char **argv) {
	node_t root, *current, *current_subclass;
	dev_node_t dev_root, *current_vendor, *current_device;
	FILE *fp, *src, *hdr;
	class_node_t *class;
	dev_node_t *dev_node;
	char str[1024];
	char *token, *hex, *name;
	uint64_t line_count = 1;

	/*Init*/
	root.count = 0;
	root.children = NULL;
	dev_root.base.count = 0;
	dev_root.base.children = NULL;

	if(argc < 4) {
		printf("USAGE: %s <INPUT_PCI_IDS> <SRC_OUTPUT> <HDR_OUTPUT>\n", argv[0]);
		return 1;
	}

	fp = fopen(argv[1], "r");
	if(!fp) {
		printf("Unable to open file %s: %s\n", argv[1], strerror(errno));
		return 1;
	}

	src = fopen(argv[2], "w");
	if(!src) {
		fclose(fp);
		printf("Unable to open file %s for writing: %s\n", argv[2], strerror(errno));
		return 1;
	}

	hdr = fopen(argv[3], "w");
	if(!hdr) {
		fclose(fp);
		fclose(src);
		printf("Unable to open file %s for writing: %s\n", argv[3], strerror(errno));
		return 1;
	}

	while(fgets(str, 1000, fp) != 0) {
		token = strtok(str, " \n");
		if(token == NULL || token[0] == '#') {
			line_count++;
			continue;	
		} 
		else if (token[0] == 'C') {
			hex = strtok(NULL, " ");
			name = strtok(NULL, "\n");
			uint64_t offset = 0;
			uint64_t len = 1;
			if(name[0] == ' ') {
				name++;
			}
			
			while(strcspn(&name[offset], "\"?") != strlen(&name[offset])) {
				offset += strcspn(&name[offset], "\"") + 1;
				len++;
			}

			class = calloc(1, sizeof(class_node_t) + strlen(name) + len);

			class->code = strtoull(hex, NULL, 16);
			memcpy(class->str, name, strlen(name));

			offset = 0;
			while(strcspn(&class->str[offset], "\"?") != strlen(&class->str[offset])) {
				offset += strcspn(&class->str[offset], "\"?");
				memmove(&class->str[offset + 1], &class->str[offset], strlen(&class->str[offset]));
				class->str[offset] = '\\';
				offset += 2;
			}
			class->str[strcspn(class->str, "\n")] = 0;
			
			insert_node(&root, (void*)class);
			current = (void*)class;
		} 
		else if(token[0] == '\t' && is_hex(token[1]) && is_hex(token[2]) && token[3] == 0) {
			hex = &token[1];
			name = strtok(NULL, "\n");
			uint64_t offset = 0;
			uint64_t len = 1;
			if(name[0] == ' ') {
				name++;
			}
			
			while(strcspn(&name[offset], "\"?") != strlen(&name[offset])) {
				offset += strcspn(&name[offset], "\"") + 1;
				len++;
			}

			class = calloc(1, sizeof(class_node_t) + strlen(name) + len);

			class->code = strtoull(hex, NULL, 16);
			memcpy(class->str, name, strlen(name));

			offset = 0;
			while(strcspn(&class->str[offset], "\"?") != strlen(&class->str[offset])) {
				offset += strcspn(&class->str[offset], "\"?");
				memmove(&class->str[offset + 1], &class->str[offset], strlen(&class->str[offset]));
				class->str[offset] = '\\';
				offset += 2;
			}

			class->str[strcspn(class->str, "\n")] = 0;
			insert_node(current, (void*)class);
			current_subclass = (node_t*)class;
		} 
		else if(token[0] == '\t' && token[1] == '\t' && is_hex(token[2]) && is_hex(token[3]) && token[4] == 0) {
			hex = &token[2];
			name = strtok(NULL, "\n");
			uint64_t len = 1;
			uint64_t offset = 0;

			if(name[0] == ' ') {
				name++;
			}
		
			while(strcspn(&name[offset], "\"?") != strlen(&name[offset])) {
				offset += strcspn(&name[offset], "\"") + 1;
				len++;
			}

			class = calloc(1, sizeof(class_node_t) + strlen(name) + len);

			class->code = strtoull(hex, NULL, 16);
			memcpy(class->str, name, strlen(name));

			offset = 0;
			while(strcspn(&class->str[offset], "\"?") != strlen(&class->str[offset])) {
				offset += strcspn(&class->str[offset], "\"?");
				memmove(&class->str[offset + 1], &class->str[offset], strlen(&class->str[offset]));
				class->str[offset] = '\\';
				offset += 2;
			}

			class->str[strcspn(class->str, "\n")] = 0;
			
			insert_node(current_subclass, (node_t*)class);
		} 
		else if(is_hex(token[0]) && is_hex(token[1]) 
				&& is_hex(token[2]) && is_hex(token[3])) {
			hex = token;
			name = strtok(NULL, "\n");
			uint64_t len = 1;
			uint64_t offset = 0;
			
			if(name[0] == ' ') {
				name++;
			}

			while(strcspn(&name[offset], "\"?") != strlen(&name[offset])) {
				offset += strcspn(&name[offset], "\"") + 1;
				len++;
			}

			dev_node = calloc(1, sizeof(dev_node_t) + strlen(name) + len); 
			if(!dev_node) {
				return -1;
			}

			dev_node->code = strtoull(hex, NULL, 16);
			memcpy(dev_node->str, name, strlen(name));
			
			/*Clean " & ? from the strings*/
			offset = 0;
			while(strcspn(&dev_node->str[offset], "\"?") != strlen(&dev_node->str[offset])) {
				offset += strcspn(&dev_node->str[offset], "\"?");
				memmove(&dev_node->str[offset + 1], &dev_node->str[offset], strlen(&dev_node->str[offset]));
				dev_node->str[offset] = '\\';
				offset += 2;
			}

			/*Remove New line if there is one*/
			dev_node->str[strcspn(dev_node->str, "\n")] = 0;


			insert_node((node_t*)&dev_root, (void*)dev_node);
			current_vendor = dev_node;
		
		} 
		else if(token[0] == '\t' && is_hex(token[1]) && is_hex(token[2])
				&& is_hex(token[3]) && is_hex(token[4])) {
			hex = &token[1];
			name = strtok(NULL, "\n");
			uint64_t len = 1;
			uint64_t offset = 0;

			if(name[0] == ' ') {
				name++;
			}

			while(strcspn(&name[offset], "\"?") != strlen(&name[offset])) {
				offset += strcspn(&name[offset], "\"") + 1;
				len++;
			}

			dev_node = calloc(1, sizeof(dev_node_t) + strlen(name) + len); 
			if(!dev_node) {
				return -1;
			}

			dev_node->code = strtoull(hex, NULL, 16);
			memcpy(dev_node->str, name, strlen(name));
			
			/*Clean " & ? from the strings*/
			offset = 0;
			while(strcspn(&dev_node->str[offset], "\"?") != strlen(&dev_node->str[offset])) {
				offset += strcspn(&dev_node->str[offset], "\"?");
				memmove(&dev_node->str[offset + 1], &dev_node->str[offset], strlen(&dev_node->str[offset]));
				dev_node->str[offset] = '\\';
				offset += 2;
			}


			dev_node->str[strcspn(dev_node->str, "\n")] = 0;
			insert_node((void*)current_vendor, (void*)dev_node);
			current_device = dev_node;
		} 
		else if(token[0] == '\t' && token[1] == '\t' && is_hex(token[2]) 
				&& is_hex(token[3]) && is_hex(token[4]) && is_hex(token[5])) {
			/*Sub Vendor Stuffs*/
		} else {
			printf("Line %lu contains unknown error\n", line_count);
			return 1;
		}
		line_count++;
	}
	fclose(fp);
	
	output_generate_headers(src, hdr);
	output_generate_class_func(&root, src, hdr);
	output_generate_subclass_func(&root, src, hdr);
	output_generate_progif_func(&root, src, hdr);
	output_generate_vendor_func(NULL, src, hdr);
	output_generate_device_func(NULL, src, hdr);
	fclose(hdr);
	fclose(src);

	for(uint64_t i = 0; i < root.count; i++) {
		class_node_t *class = (class_node_t*) root.children[i];
		for(uint64_t j = 0; j < class->base.count; j++) {
			class_node_t *subclass = (class_node_t*) class->base.children[j];
			for (uint64_t k = 0; k < subclass->base.count; k++) {
				class_node_t *pi = (class_node_t*)subclass->base.children[k];
				free(pi);
			}
			if(subclass->base.count && subclass->base.children) {
				free(subclass->base.children);
			}
			free(subclass);
		}
		if(class->base.count && class->base.children) {
			free(class->base.children);
		}
		free(class);
	}
	free(root.children);

	for(uint64_t i = 0; i < dev_root.base.count; i++) {
		dev_node_t *vendor = (void*)dev_root.base.children[i];
			

		for(uint64_t j = 0; j < vendor->base.count; j++) {
			dev_node_t *device = (void*)vendor->base.children[j];
			free(device);
		}
		if(vendor->base.count && vendor->base.children) {
			free(vendor->base.children);
		}
		free(vendor);
	}
	free(dev_root.base.children);
	return EXIT_SUCCESS;
}
