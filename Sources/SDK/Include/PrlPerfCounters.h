/*
 * PrlPerfCounters.h: Published performance counters list
 *
 * Copyright (C) 1999-2014 Parallels IP Holdings GmbH
 *
 * This file is part of Parallels SDK. Parallels SDK is free
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
 * Our contact details: Parallels IP Holdings GmbH, Vordergasse 59, 8200
 * Schaffhausen, Switzerland; http://www.parallels.com/.
 */


#ifndef __PARALLELS_PERF_COUNTERS_H__
#define __PARALLELS_PERF_COUNTERS_H__

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
#define PRL_NET_CLASSFUL_TRAFFIC_PTRN		"net.classful.traffic"

//Guest OS CPU usage in percents (modulated to 100%)
#define PRL_GUEST_CPU_USAGE_PTRN			"guest.cpu.usage"
#define PRL_GUEST_CPU_TIME_PTRN				"guest.cpu.time"
#define PRL_HOST_CPU_TIME_PTRN				"host.cpu.time"

//Guest OS RAM usage in megabytes
#define PRL_GUEST_RAM_USAGE_PTRN			"guest.ram.usage"
#define PRL_GUEST_RAM_CACHED_PTRN			"guest.ram.cached"
#define PRL_WS_RECLAIMABLE_PTRN				"ws.reclaimable"

#endif

