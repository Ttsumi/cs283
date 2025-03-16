1. How does the remote client determine when a command's output is fully received from the server, and what techniques can be used to handle partial reads or ensure complete message transmission?

The remote client determines that a command's output is fully received by using message delimiters like null termination. To ensure partial reads or ensure complete message transmission, we can use: Delimiters, Fixed byte length, signals or by checking if the connection has been closed.

2. This week's lecture on TCP explains that it is a reliable stream protocol rather than a message-oriented one. Since TCP does not preserve message boundaries, how should a networked shell protocol define and detect the beginning and end of a command sent over a TCP connection? What challenges arise if this is not handled correctly?

A networked shell protocal defines and detects the beginning and end of a command using null terminated strings, and by defining fixed length messages. If they are not handled correctly, we could end up with partial reads.

3. Describe the general differences between stateful and stateless protocols.

Stateful protocols maintain information across requests from the same client whereas stateless protocols do not store information and each request is treated independently.

4. Our lecture this week stated that UDP is "unreliable". If that is the case, why would we ever use it?

UDP is a message oriented protocol where individual messages are packaged and delivered. However it is not reliable since messages can get lost. Even still there are uses for it, especially when speed is more important that reliability.

5. What interface/abstraction is provided by the operating system to enable applications to use network communications?

Sockets are provided by the OS to enable applications to use network communications.
