# Several RTOS Note 

note from Mastering the FreeRTOS Real Time Kernel 

**7.7.2 Considerations When Using a Queue From an ISR**

Queues provide an easy and convenient way of passing data from an interrupt to a task, but it is not efficient
to use a queue if data is arriving at a high frequency.
Many of the demo applications in the FreeRTOS download include a simple UART driver that uses a queue to
pass characters out of the UART's receive ISR. In those demos a queue is used for two reasons: to demonstrate
queues being used from an ISR, and to deliberately load the system in order to test the FreeRTOS port. The
ISRs that use a queue in this manner are definitely not intended to represent an efficient design, and unless the
data is arriving slowly, it is recommended that production code does not copy this technique. More efficient
techniques, that are suitable for production code, include

---

Using Direct Memory Access (DMA) hardware to receive and buffer characters. This method has
practically no software overhead. A direct to task notification[^20] can then be used to unblock the task
that will process the buffer only after a break in transmission has been detected.
[^20]: Direct to task notifications provide the most efficient method of unblocking a task from an ISR.
Direct to task notifications are covered in Chapter 9, Task Notifications.

---

Copying each received character into a thread safe RAM buffer[^21]. Again, a direct to task notification
can be used to unblock the task that will process the buffer after a complete message has been received,
or after a break in transmission has been detected

---
