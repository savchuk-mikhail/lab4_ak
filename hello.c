// SPDX-License-Identifier: GPL-2.0
#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/moduleparam.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/ktime.h>
#include <linux/errno.h>
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Student");
MODULE_DESCRIPTION("Kernel module for logging hello events");
/* Параметр кількості повідомлень */
static unsigned int repeat = 1;
module_param(repeat, uint, 0444);
MODULE_PARM_DESC(repeat, "Number of greetings to display");
/* Дані про кожну подію */
struct greeting_record {
 struct list_head node;
 ktime_t timestamp;
};
static LIST_HEAD(greeting_history);
/* Очищення списку подій */
static void release_records(void)
{
 struct greeting_record *record, *next;
list_for_each_entry_safe(record, next,
    &greeting_history, node) {
 pr_info("Event timestamp: %lld ns\n",
  (long long)ktime_to_ns(record->timestamp));
 list_del(&record->node);
 kfree(record);
}
}
/* Завантаження модуля */
static int __init greeting_start(void)
{
 unsigned int index;
 struct greeting_record *record;
if (repeat > 10) {
 pr_err("Invalid repeat value: %u\n", repeat);
 return -EINVAL;
}

if (repeat == 0 || repeat >= 5)
 pr_warn("Warning: repeat = %u\n", repeat);

for (index = 0; index < repeat; index++) {
 record = kmalloc(sizeof(*record), GFP_KERNEL);
 if (!record) {
  release_records();
  return -ENOMEM;
 }

 record->timestamp = ktime_get();
 list_add_tail(&record->node, &greeting_history);

 pr_info("Hello, world!\n");
}

return 0;
}
/* Вивантаження модуля */
static void __exit greeting_stop(void)
{
 release_records();
 pr_info("Greeting module removed\n");
}
module_init(greeting_start);
module_exit(greeting_stop);
