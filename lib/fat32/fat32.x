define show_pool
	set $i = 0
	set $pool_size = 14

	printf "Pool content\n"
	printf "============\n"
	while $i < $pool_size
		printf "pool[%2d] --> dirty: %d\t\tpage: %7d\t\tuse: %3d\n", $i, pool[$i].dirty, pool[$i].page, pool[$i].use
		set $i = $i + 1
	end
end

document show_pool
Show buffer pool usage
end

define show_buffer
	set $i = $arg0
	set $j = 0

	printf "Buffer content\n"
	printf "==============\n"
	printf "pool[%d]:\n", $i
	printf "\tdirty: %d   page: %d   use: %d\n", pool[$i].dirty, pool[$i].page, pool[$i].use
	printf "\tcontent:"
	while $j < 512
		if $j % 16 == 0
			printf "\n\t"
		end
		set $c=pool[$i].content[$j]
		if ($c >= 0x20) && ($c < 0x80)
			printf " %c  ", $c
		else
			printf "(%02x)", $c
		end

		set $j = $j + 1
	end
	printf "\n"
end

document show_buffer
Show buffer content (from FAT32 library)
end

define show_fat32
	printf "FAT32 structure\n"
	printf "===============\n"
	printf "Boot sector: %d (%x) -> byte %d (%x)\n", fat.boot_sect, fat.boot_sect, fat.boot_sect * 512, fat.boot_sect * 512
	printf "Number of FAT: %d\n", fat.num_fat
	printf "Sectors per FAT: %d\n", fat.sect_per_fat
	printf "Sectors per cluster: %d\n", fat.sect_per_clust
	printf "Number of sectors: %d\n", fat.num_sect
	printf "Maximum cluster index: %d\n", fat.max_clust
	printf "FAT start sector: %d (%x) -> byte %d (%x)\n", fat.start, fat.start, fat.start * 512, fat.start * 512
	printf "Root directory start sector: %d (%x) -> byte %d (%x)\n", fat.root_start, fat.root_start, fat.root_start * 512, fat.root_start * 512
	printf "Data start sector: %d (%x) -> byte %d (%x)\n", fat.data_start, fat.data_start, fat.data_start * 512, fat.data_start * 512
end

document show_fat32
Show FAT32 structure
end

define show_file
	printf "File structure\n"
	printf "==============\n"
	printf "Filename: %s\n", $arg0.dir.name
	printf "Extension: %s\n", $arg0.dir.ext
	printf "Date: %d/%d/%d\n", $arg0.dir.day, $arg0.dir.month, $arg0.dir.year
	printf "Time: %d:%d:%d\n", $arg0.dir.hour, $arg0.dir.minutes, $arg0.dir.seconds
	printf "Size: %d\n", $arg0.dir.size
	printf "Start cluster: %d\n", $arg0.dir.start_cluster
	printf "Current cluster: %d\n", $arg0.current_cluster
	printf "Current sector: %d\n", $arg0.current_sector
	printf "Sector index: %d\n", $arg0.sector_index
	printf "Descriptor sector: %d\n", $arg0.descriptor_sector
	printf "Descriptor index: %d\n", $arg0.descriptor_index
end

document show_file
Show file structure
end

define compute_cluster_address
	set $a = fat.data_start + ($arg0 - 2) * fat.sect_per_clust
	printf "Cluster %d (%x) -> Page %d (0x%x) -> Byte %d (%x)\n", $arg0, $arg0, $a, $a, $a * 512, $a * 512
end

document compute_cluster_address
Compute address of first of a given sector
end
