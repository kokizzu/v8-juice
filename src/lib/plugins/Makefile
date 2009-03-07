include toc2.make
all: subdirs

package.subdirs = stringstuff whio

ifeq (1,$(v8_juice_plugin_CONFIG_HAVE_NCURSES))
package.subdirs += ncurses
endif

ifeq (1,$(v8_juice_plugin_CONFIG_HAVE_SQLITE3))
package.subdirs += sqlite3
endif

subdirs: $(addprefix subdir-,$(package.subdirs))

package.dist_files += plugin_template.make
