# How does it work

## Architecture

c.f. [doc](https://ibkrcampus.com/ibkr-api-page/twsapi-doc/#architecture)

- `EWrapper`: the interface used by TWS to deliver information to the API client application. By implementing this interface the client will be able to reveice and handle information coming from TWS.
    - `nextValidID`: member used to indicate that the connection is completed and other messages can be sent from the API client to TWS.
- `EClientSocket`: class used to send messages to TWS, takes a EReaderSignal object as a constructor. It is not overriden. We directling used its members to send request to TWS.
    - `eConnect`: member used to make the connection
- `EReaderSignal`: object used in APIs to signal a message is ready for processing in the queue.
- `EReader`:  class which reads and parses raw messages from TWS. It is initialized after the connection is made. It uses its own thread.

The API programs have 3 threads: a reader thread, a queue thread, and then a wrapper thread.

Test fails on ST_CONTRACTOPERATION_ACK where msg is always empty

Do not put breakpoints in the tws_api source code (segmentation fails)

Initial handshake msg id
5 OPEN_ORDER 
3 ORDER_STATUS
5 OPEN_ORDER 
3 ORDER_STATUS
53 OPEN_ORDER_END
15 MANAGED_ACCTS
9 NEXT_VALID_ID
4 ERR_MSG (multiple times)

4 ERR_MSG
58 MARKET_DATA_TYPE
4 ERR_MSG
1 TICK_PRICE
1 TICK_PRICE
2 TICK_SIZE
46 TICK_STRING

4 ERR_MSG

81 TICK_REQ_PARAMS

reqTickByTickData>  processTickByTickDataMsg 99