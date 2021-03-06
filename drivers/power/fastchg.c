/*
 * Author: Chad Froebel <chadfroebel@gmail.com>
 *
 * Ported to LG G3 : Jeremy Neuberger <ptmr323@gmail.com>
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

/*
 * Forced Fast Charge - SysFS interface :
 * --------------------------------------
 *
 * /sys/kernel/fast_charge/force_fast_charge (rw)
 *
 *   0 - disabled (default)
 *   1 - substitute AC to USB
 *   2 - use custom mA configured through sysfs interface (see below)
 *
 * /sys/kernel/fast_charge/ac_charge_level (rw)
 *
 *   rate at which to charge when on AC (1.0A/h to 2.0A/h)
 *
 * /sys/kernel/fast_charge/usb_charge_level (r/w)
 *
 *   rate at which to charge when on USB (0.5A/h to 1.0A/h)
 *
 * /sys/kernel/fast_charge/failsafe (rw)
 *
 *   0 - disabled - allow 0.1A/h up to 2.0A/h to be used as AC / USB custom current
 *   1 - enabled  - behaviour as described above (default)
 *
 * /sys/kernel/fast_charge/ac_levels (ro)
 *
 *   display available levels for AC (for failsafe enabled mode)
 *
 * /sys/kernel/fast_charge/usb_levels (ro)
 *
 *   display available levels for USB (for failsafe enabled mode)
 *
 * /sys/kernel/fast_charge/version (ro)
 *
 *   display fast charge version information
 *
 * /sys/kernel/fast_charge/info (ro)
 *
 *   display complete fast charge configuration in human readable format
 *
 */

#include <linux/kobject.h>
#include <linux/sysfs.h>
#include <linux/fastchg.h>

int current_charge_level;

int force_fast_charge = 2;

/* sysfs interface for "force_fast_charge" */
static ssize_t force_fast_charge_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	return sprintf(buf, "%d\n", force_fast_charge);
}

static ssize_t force_fast_charge_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)
{

	int new_force_fast_charge;

	sscanf(buf, "%du", &new_force_fast_charge);

	switch(new_force_fast_charge) {
		case FAST_CHARGE_DISABLED:
		case FAST_CHARGE_FORCE_AC:
		case FAST_CHARGE_FORCE_CUSTOM_MA:		force_fast_charge = new_force_fast_charge;
								return count;
		default:					return -EINVAL;
	}
}

static struct kobj_attribute force_fast_charge_attribute =
__ATTR(force_fast_charge, 0666, force_fast_charge_show, force_fast_charge_store);

/* sysfs interface for "ac_charge_level" */

int ac_charge_level = 2400;

static ssize_t ac_charge_level_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	return sprintf(buf, "%d\n", ac_charge_level);
}

static ssize_t ac_charge_level_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)
{

	int new_ac_charge_level;

	sscanf(buf, "%du", &new_ac_charge_level);

	if (failsafe == FAIL_SAFE_DISABLED && new_ac_charge_level <= MAX_CHARGE_LEVEL && new_ac_charge_level >= MIN_CHARGE_LEVEL) {

		ac_charge_level = new_ac_charge_level;
		return count;

	}

	else {

		switch (new_ac_charge_level) {
			case AC_CHARGE_1000:
			case AC_CHARGE_1100:
			case AC_CHARGE_1200:
			case AC_CHARGE_1300:
			case AC_CHARGE_1400:
			case AC_CHARGE_1500:
			case AC_CHARGE_1600:
			case AC_CHARGE_1700:
			case AC_CHARGE_1800:
			case AC_CHARGE_1900:
			case AC_CHARGE_2000:
			case AC_CHARGE_2100:
			case AC_CHARGE_2200:
			case AC_CHARGE_2300:
			case AC_CHARGE_2400:		ac_charge_level = new_ac_charge_level;
							return count;
			default:			return -EINVAL;

		}

	}

	return -EINVAL;

}

static struct kobj_attribute ac_charge_level_attribute =
__ATTR(ac_charge_level, 0666, ac_charge_level_show, ac_charge_level_store);

/* sysfs interface for "usb_charge_level" */

int usb_charge_level = 1500;

static ssize_t usb_charge_level_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	return sprintf(buf, "%d\n", usb_charge_level);
}

static ssize_t usb_charge_level_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)
{

	int new_usb_charge_level;

	sscanf(buf, "%du", &new_usb_charge_level);

	if (failsafe == FAIL_SAFE_DISABLED && new_usb_charge_level <= MAX_CHARGE_LEVEL && new_usb_charge_level >= MIN_CHARGE_LEVEL) {

		usb_charge_level = new_usb_charge_level;
		return count;

	}

	else {

		switch (new_usb_charge_level) {
			case USB_CHARGE_500:
			case USB_CHARGE_600:
			case USB_CHARGE_700:
			case USB_CHARGE_800:
			case USB_CHARGE_900:
			case USB_CHARGE_1000:
			case USB_CHARGE_1100:
			case USB_CHARGE_1200:
			case USB_CHARGE_1300:
			case USB_CHARGE_1400:
			case USB_CHARGE_1500:		usb_charge_level = new_usb_charge_level;
							return count;
			default:			return -EINVAL;
		}

	}

	return -EINVAL;

}

static struct kobj_attribute usb_charge_level_attribute =
__ATTR(usb_charge_level, 0666, usb_charge_level_show, usb_charge_level_store);

/* sysfs interface for "failsafe" */

int failsafe;

static ssize_t failsafe_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	return sprintf(buf, "%d\n", failsafe);
}

static ssize_t failsafe_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)
{

	int new_failsafe;

	sscanf(buf, "%du", &new_failsafe);

	switch (new_failsafe) {
		case FAIL_SAFE_ENABLED:		usb_charge_level = USB_CHARGE_500;
						ac_charge_level = AC_CHARGE_1500;
						failsafe = new_failsafe;
						return count;
		case FAIL_SAFE_DISABLED:	failsafe = new_failsafe;
						return count;
		default:			return -EINVAL;
	}

}

static struct kobj_attribute failsafe_attribute =
__ATTR(failsafe, 0666, failsafe_show, failsafe_store);

/* sysfs interface for "ac_levels" */
static ssize_t ac_levels_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	return sprintf(buf, "%s\n", AC_LEVELS);
}

static struct kobj_attribute ac_levels_attribute =
__ATTR(ac_levels, 0444, ac_levels_show, NULL);

/* sysfs interface for "usb_levels" */
static ssize_t usb_levels_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	return sprintf(buf, "%s\n", USB_LEVELS);
}

static struct kobj_attribute usb_levels_attribute =
__ATTR(usb_levels, 0444, usb_levels_show, NULL);

/* sysfs interface for "info" */
static ssize_t info_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	return sprintf(
		buf,
		"Forced Fast Charge for %s %s\n\n"
		"Fast charge mode      : %s\n"
		"Custom  AC level      : %dmA/h\n"
		"Custom USB level      : %dmA/h\n"
		"Current charge level  : %dmA/h\n"
		"Failsafe mode         : %s\n"
		"Valid AC  levels      : %s\n"
		"Valid USB levels      : %s\n",
		 DEVICE_NAME,
		 FAST_CHARGE_VERSION,
		 force_fast_charge == FAST_CHARGE_DISABLED 	   ? "0 - Disabled (default)" :
		(force_fast_charge == FAST_CHARGE_FORCE_AC         ? "1 - Use stock AC level on USB" :
		(force_fast_charge == FAST_CHARGE_FORCE_CUSTOM_MA  ? "2 - Use custom mA on AC and USB" : "Problem : value out of range")),
		 ac_charge_level,
		 usb_charge_level,
		 current_charge_level,
		 failsafe          == FAIL_SAFE_DISABLED           ? "0 - Failsafe disabled - please be careful !" :
		(failsafe          == FAIL_SAFE_ENABLED            ? "1 - Failsafe active (default)" : "Problem : value out of range"),
		 failsafe          == FAIL_SAFE_ENABLED            ? AC_LEVELS : ANY_LEVELS,
		 failsafe          == FAIL_SAFE_ENABLED            ? USB_LEVELS : ANY_LEVELS
		);
}

static struct kobj_attribute info_attribute =
__ATTR(info, 0444, info_show, NULL);

/* sysfs interface for "version" */
static ssize_t version_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	return sprintf(buf, "%s\n", FAST_CHARGE_VERSION);
}

static struct kobj_attribute version_attribute =
__ATTR(version, 0444, version_show, NULL);

/* Initialize fast charge sysfs folder */
static struct kobject *force_fast_charge_kobj;

static struct attribute *force_fast_charge_attrs[] = {
	&force_fast_charge_attribute.attr,
	&ac_charge_level_attribute.attr,
	&usb_charge_level_attribute.attr,
	&failsafe_attribute.attr,
	&ac_levels_attribute.attr,
	&usb_levels_attribute.attr,
	&info_attribute.attr,
	&version_attribute.attr,
	NULL,
};

static struct attribute_group force_fast_charge_attr_group = {
.attrs = force_fast_charge_attrs,
};

int force_fast_charge_init(void)
{
	int force_fast_charge_retval;

	force_fast_charge     = FAST_CHARGE_FORCE_CUSTOM_MA; /* Forced fast charge disabled by default */
	ac_charge_level       = AC_CHARGE_2400; 	      /* Default AC charge level to 1500mA/h    */
	usb_charge_level      = USB_CHARGE_1500;	      /* Default USB charge level to 500mA/h    */
	current_charge_level  = NOT_FAST_CHARGING;         /* Default to not fast charging           */
	failsafe              = FAIL_SAFE_ENABLED;    /* Allow only values in list by default   */

        force_fast_charge_kobj = kobject_create_and_add("fast_charge", kernel_kobj);
        if (!force_fast_charge_kobj) {
                return -ENOMEM;
        }
        force_fast_charge_retval = sysfs_create_group(force_fast_charge_kobj, &force_fast_charge_attr_group);
        if (force_fast_charge_retval)
                kobject_put(force_fast_charge_kobj);
        return (force_fast_charge_retval);
}
/* end sysfs interface */

void force_fast_charge_exit(void)
{
	kobject_put(force_fast_charge_kobj);
}

module_init(force_fast_charge_init);
module_exit(force_fast_charge_exit);

