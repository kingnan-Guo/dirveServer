#pragma once

#include "my_op.h"

void _device_create(int minor);
void _device_destroy(int minor);
void _register_device_operations(struct my_operations *opr);