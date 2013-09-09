define show_list
	set $n = $arg0->uxNumberOfItems
	set $cpt = 0
	printf "\t%d items in the list\n\t", $n
	set $l = $arg0->pxIndex
	while $cpt < $n
		set $l = $l->pxNext
		printf "-> (0x%x, \"%s\") ", $l->xItemValue, ((tskTCB*)($l->pvOwner))->pcTaskName
		set $cpt = $cpt + 1
	end
	if $cpt != 0
		printf "\n"
	else
		printf "\r"
	end
end

document show_list
Show FreeRTOS list content
end

define show_queue
	set $q = (xQUEUE*)$arg0
	printf "Queue status\n"
	printf "============\n"
	printf "Head:    0x%x     Tail:     0x%x\n", $q->pcHead, $q->pcTail
	printf "WriteTo: 0x%x     ReadFrom: 0x%x\n", $q->pcWriteTo, $q->pcReadFrom
	printf "Tasks waiting to send:\n"
	show_list $q->xTasksWaitingToSend
	printf "Tasks waiting to receive:\n"
	show_list $q->xTasksWaitingToReceive
	printf "Queue Length; %d\n", $q->uxLength
	printf "Number of items in the queue: %d\n", $q->uxMessagesWaiting
	printf "Item size: %d\n", $q->uxItemSize
end

document show_queue
Show FreeRTOS queue status
end

define _show_task
	set $task = (tskTCB *) $arg0
	set $task_pc = *($task->pxTopOfStack + 14)
	printf "%x\n", $task_pc
	p/a $task_pc
	
end

define _show_task_list
	# arg0 should be a task list pointer
	set $list = (xList*) $arg0
	
	if $list->uxNumberOfItems == 0
		printf "\tempty\n"
	else
		# Start from the first after the ent
		set $item = $list->xListEnd.pxNext
		while $item != &($list->xListEnd)
			# Get the task (the owner)
			set $task = (tskTCB *)($item->pvOwner)
			
			# print the name
			printf "\t%s\t", $task->pcTaskName
			
			# print the task information
			_show_task $task
			
			# Move forward
			set $item = $item->pxNext
		end
	end
end



define show_all_tasks
	printf "---------------------\n"
	printf "Debugging Tasks State\n"
	printf "---------------------\n"
		
	printf "\n\tTask\tPC\n"
	
	printf "Running Task:\n"
	printf "\t%s\t%x\n", pxCurrentTCB->pcTaskName, $pc
	p/a $pc

	
	# Loop over the priorities
	set $prio = 0
	while $prio <= uxTopUsedPriority
		set $list = &pxReadyTasksLists[ $prio ]
		if $list->uxNumberOfItems > 0
			printf "Priority #%u Task List\n", $prio
			_show_task_list $list
		end
		set $prio = $prio + 1
	end
	
	if pxDelayedTaskList->uxNumberOfItems > 0
		printf "Delayed Task List:\n"
		_show_task_list pxDelayedTaskList
	end
	
	if pxOverflowDelayedTaskList->uxNumberOfItems > 0
		printf "Overflow Delayed Task List:\n"
		_show_task_list pxOverflowDelayedTaskList
	end
	
	if xSuspendedTaskList.uxNumberOfItems > 0
		printf "Checking Suspended List\n"
		_show_task_list &xSuspendedTaskList
	end
	
end
