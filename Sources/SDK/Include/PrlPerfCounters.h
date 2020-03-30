/*
 * PrlPerfCounters.h: Published performance counters list
 *
 * Copyright (c) 1999-2017, Parallels International GmbH
 * Copyright (c) 2017-2019 Virtuozzo International GmbH. All rights reserved.
 *
 * This file is part of Virtuozzo SDK. Virtuozzo SDK is free
 * software; you can redistribute it and/or modify it under the
 * terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License,
 * or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.  If not, see
 * <http://www.gnu.org/licenses/> or write to Free Software Foundation,
 * 51 Franklin Street, Fifth Floor Boston, MA 02110, USA.
 *
 * Our contact details: Virtuozzo International GmbH, Vordergasse 59, 8200
 * Schaffhausen, Switzerland; http://www.virtuozzo.com/.
 */


#ifndef __VIRTUOZZO_PERF_COUNTERS_H__
#define __VIRTUOZZO_PERF_COUNTERS_H__

//Total count of read requests to IDE controller
#define PRL_IDE_READ_REQUESTS_PTRN			"devices.ide#.read_requests"

//Total count of read bytes through IDE controller
#define PRL_IDE_READ_TOTAL_PTRN				"devices.ide#.read_total"

//Total count of write requests to IDE controller
#define PRL_IDE_WRITE_REQUESTS_PTRN			"devices.ide#.write_requests"

//Total count of write bytes through IDE controller
#define PRL_IDE_WRITE_TOTAL_PTRN			"devices.ide#.write_total"

//Total count of read requests to SCSI controller
#define PRL_SCSI_READ_REQUESTS_PTRN			"devices.scsi#.read_requests"

//Total count of read bytes through SCSI controller
#define PRL_SCSI_READ_TOTAL_PTRN			"devices.scsi#.read_total"

//Total count of write requests to SCSI controller
#define PRL_SCSI_WRITE_REQUESTS_PTRN			"devices.scsi#.write_requests"

//Total count of write bytes through SCSI controller
#define PRL_SCSI_WRITE_TOTAL_PTRN			"devices.scsi#.write_total"

//Total count of read requests to SATA controller
#define PRL_SATA_READ_REQUESTS_PTRN			"devices.sata#.read_requests"

//Total count of read bytes through SATA controller
#define PRL_SATA_READ_TOTAL_PTRN			"devices.sata#.read_total"

//Total count of write requests to SATA controller
#define PRL_SATA_WRITE_REQUESTS_PTRN			"devices.sata#.write_requests"

//Total count of write bytes through SATA controller
#define PRL_SATA_WRITE_TOTAL_PTRN			"devices.sata#.write_total"



#define PRL_NET_PKTS_IN_PTRN				"net.nic#.pkts_in"
#define PRL_NET_PKTS_OUT_PTRN				"net.nic#.pkts_out"
#define PRL_NET_BYTES_IN_PTRN				"net.nic#.bytes_in"
#define PRL_NET_BYTES_OUT_PTRN				"net.nic#.bytes_out"


//Classful network traffic counting info. Result parameter provides
//as binary buffer contains structure PRL_STAT_NET_TRAFFIC. See
//PrlIOStructs.h for more info
#define PRL_NET_CLASSFUL_TRAFFIC_PTRN			"net.classful.traffic"
#define PRL_NET_CLASSFUL_TRAFFIC_IPV4_PTRN		"net.classful.traffic.ipv4"
#define PRL_NET_CLASSFUL_TRAFFIC_IPV6_PTRN		"net.classful.traffic.ipv6"

//Guest OS CPU usage in percents (modulated to 100%)
#define PRL_GUEST_CPU_USAGE_PTRN			"guest.cpu.usage"

// The sum of guest time changes since the last query (delta) for every vCPU averaged on
// host CPU number, in microseconds
#define PRL_GUEST_CPU_TIME_PTRN				"guest.cpu.time"
// Host time change since the last query, in microseconds
#define PRL_HOST_CPU_TIME_PTRN				"host.cpu.time"

// per vcpu statistics in nanosecs
#define PRL_GUEST_VCPU_TIME_PTRN			"guest.vcpu#.time"

//Guest OS RAM usage in megabytes
#define PRL_GUEST_RAM_USAGE_PTRN			"guest.ram.usage"
#define PRL_GUEST_RAM_CACHED_PTRN			"guest.ram.cached"
#define PRL_GUEST_RAM_TOTAL_PTRN			"guest.ram.total"

//Guest OS virtual memory stats in counts
#define PRL_GUEST_RAM_SWAP_IN				"guest.ram.swap_in"
#define PRL_GUEST_RAM_SWAP_OUT				"guest.ram.swap_out"
#define PRL_GUEST_RAM_MINOR_FAULT			"guest.ram.minor_fault"
#define PRL_GUEST_RAM_MAJOR_FAULT			"guest.ram.major_fault"
//Guest OS balloon sizes in megabytes
#define PRL_GUEST_RAM_BALLOON_ACTUAL			"guest.ram.balloon_actual"
//Guest OS estimate of how much memory is available, without swapping
#define PRL_GUEST_RAM_AVAILABLE				"guest.ram.available"

// Mounted filesystems statistics
// Filesystem device name, as it is seen inside guest
#define PRL_GUEST_FS_NAME_PTRN				"guest.fs#.name"
// Total size of a filesystem (KiB)
#define PRL_GUEST_FS_TOTAL_SPACE_PTRN			"guest.fs#.total"
// Amount of free space on a filesystem (KiB)
#define PRL_GUEST_FS_FREE_SPACE_PTRN			"guest.fs#.free"
// Disk indices (see PrlVmDev_GetIndex() API function)
#define PRL_GUEST_FS_DISK_INDEX_PTRN			"guest.fs#.disk.#"

#endif

