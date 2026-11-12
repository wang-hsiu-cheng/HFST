# HFST CLI 交易模擬體驗：本次修改範圍說明

Nov 12, 2026 · @王修呈

## 背景與目標

本次修改讓使用者能在兩個終端機體驗「看報價 → 下單 → 收到委託／成交回報」的完整流程，並看到 FIX 4.4 封包如何編碼與解碼。修改範圍限於 `PseudoServer`、`PseudoClient` 與新增的 `common/`，`PseudoServer_HFFT` 未更動。

修改前的狀況：

- CLI 輸出以除錯訊息為主（`pos:`、`tagNumber:`、原始封包、重複的 `Ordering`）。
- Client 只送寫死的訂單（002330、價格固定，`MakeD` 的 `OrderQty` 使用未初始化變數）。
- Server 對任何訂單都回一筆欄位寫死的 ExecutionReport（35=8），沒有報價。
- Client 主迴圈阻塞在 `std::cin`，Server 主動送出的訊息要等使用者輸入後才會被讀取。

角色分工依專題報告的架構：

| 程式 | 角色 | 本次新增的職責 |
| --- | --- | --- |
| PseudoServer | 純軟體交易所 | 產生假報價（35=W）、簡易撮合、回傳 8/9 回報 |
| PseudoClient | FPGA 端的協定前端模型 | 僅將使用者輸入（Order Out Register）編碼成 FIX，並解碼回報供顯示；不做交易決策 |

## 修改範圍總覽

共 3 個新檔、15 個修改檔。新增程式集中在行情、撮合與輸出；既有 FIX 編解碼函式只新增 overload，不改原本實作。

| 檔案 | 類型 | 變更內容 |
| --- | --- | --- |
| `common/include/ConsoleUI.h` | 新增 | 共用的彩色日誌與 FIX 封包美化輸出（header-only） |
| `PseudoServer/include/MarketSimulator.h` | 新增 | 報價模擬器與撮合引擎的宣告 |
| `PseudoServer/src/MarketSimulator.cpp` | 新增 | 隨機漫步報價、限價單撮合、改單／取消／查詢 |
| `PseudoServer/src/TaifexOrderServer.cpp` | 大幅改寫 | 輸出整理、行情執行緒、訂單交由撮合引擎、命令列參數 |
| `PseudoServer/include/TaifexOrderServer.h` | 修改 | `Session::SendMsg` 與送出鎖、header 鎖、新成員 |
| `PseudoServer/src/ServerMessenger.cpp`、`.h` | 修改 | 新增 `Make8`/`Make9` 帶值 overload 與 `MakeW`；除錯列印改為日誌 |
| `PseudoServer/src/TaifexORGenerator.cpp`、`.h` | 修改 | 新增 `SerializeW` |
| `PseudoServer/include/TaifexOrder.h` | 修改 | 新增 `FIX_W_t` 結構與 `MarketData`（W）訊息類型 |
| `PseudoClient/src/TaifexOrderClient.cpp` | 大幅改寫 | 收發執行緒拆分、指令列 REPL、回報顯示、命令列參數 |
| `PseudoClient/include/TaifexOrderClient.h` | 修改 | `Session::SendMsg`、`OrderView` 顯示快取、生命週期同步 |
| `PseudoClient/src/ClientMessenger.cpp`、`.h` | 修改 | 新增 `MakeD/F/G/H(OrderParams)` overload 與 `Receive8/9/W` 解碼；除錯列印改為日誌 |
| `PseudoClient/include/TaifexOrder.h` | 修改 | 新增 `MarketData`（W）訊息類型 |
| `PseudoServer/CMakeLists.txt`、`PseudoClient/CMakeLists.txt` | 修改 | 加入 `../common/include`、`Threads::Threads`、`MarketSimulator.cpp` |
| `README.md` | 修改 | 角色說明、指令表、範例、命令列參數 |

## 共用模組：ConsoleUI.h

`common/include/ConsoleUI.h` 是 Server 與 Client 共用的 header-only 輸出工具，只負責呈現，不碰 FIX 編解碼邏輯。

- `ui::Log(level, msg)`：執行緒安全的彩色日誌，每行帶時間戳與等級（`SESSION`、`TX`、`RX`、`ORDER`、`FILL`、`REJECT`、`MARKET`、`WARN`、`DEBUG`）。`DEBUG` 只在 `--verbose` 時輸出。
- 自動偵測：輸出不是終端機時關閉 ANSI 顏色。
- `ui::PrettyFIX()`：把封包展開成「tag｜名稱｜值｜意義」表格，例如 `54 Side 1 (BUY)`。
- `ui::OneLineFIX()`：把 SOH 換成 `|` 的單行顯示。
- `ui::FIXSummary()` / `ui::LogPacket()`：每筆收發封包印一行摘要（MsgType、seq、BodyLength、CheckSum、位元組數），`--raw` 時再加印完整 tag 表。
- `ui::ShowPrompt()` / `ui::HidePrompt()`：非同步輸出後重繪 ` >  ` 提示字元，避免與使用者輸入交錯。

## Server 端修改（PseudoServer）

Server 從「固定回一筆 8」改為「推送報價＋撮合＋依結果回 8/9」；登入、心跳、登出的會話狀態機邏輯不變。

### MarketSimulator（報價模擬）

- 標的與基準價：2330（600）、2317（110）、2454（1000）、0050（150）。
- 每個 tick（預設 1000 ms）做 −2 ～ +2 檔的隨機漫步，限制在基準價 ±10% 內。
- 價格依台股升降單位對齊：<10 為 0.01、10–50 為 0.05、50–100 為 0.1、100–500 為 0.5、500–1000 為 1、≥1000 為 5。
- bid = last，ask = last + 一檔，數量隨機。可接受 `002330` 這類補零代碼。

### MatchingEngine（簡易撮合）

| 收到 | 處理 | 回覆 |
| --- | --- | --- |
| D 新單 | 檢查代碼、數量、價格、±10%、升降單位、重複 ClOrdID；通過後掛單，若穿價則立即以對手價全部成交 | 8 NEW，接著 8 FILLED；不合格回 8 REJECTED（58 帶原因） |
| 每個 tick | 重新檢查掛單是否被穿價 | 8 FILLED |
| F 取消 | 依 OrigClOrdID 找掛單 | 8 CANCELED，找不到或已成交回 9（434=1） |
| G 改單 | 更新數量／價格（0 表示不改），再檢查撮合 | 8 REPLACED，失敗回 9（434=2） |
| H 查詢 | 依 ClOrdID 找訂單 | 8 STATUS（150=I），找不到回 9 |

每筆訂單記錄其所有 ClOrdID 對應的 OrderID，所以改單後用舊 ID 仍可取消或查詢。

### FIX 訊息新增

- `MakeW()` 與 `SerializeW()`：FIX 4.4 MarketDataSnapshotFullRefresh（35=W），欄位 262、55、268=3，接著 bid／offer／trade 三組 269/270/271。
- `Make8(FIX_8_t, …)`、`Make9(FIX_9_t, …)`：帶實際欄位值的 overload，舊版寫死欄位的 `Make8`/`Make9` 保留不動。
- BodyLength 以 `CompatBodyLength()` 計算，配合既有解碼器的長度檢查（見「已知問題」）。

### 執行緒與同步

- `Session::SendMsg()`：「組封包＋送出＋seq++」在同一把鎖內完成。接收、心跳、行情三個執行緒共用，序號不會交錯。
- `m_hdrMutex`：保護共用的 FIX header。
- 行情執行緒在登入成功後啟動，登出或斷線時停止並 join。
- `send()` 使用 `MSG_NOSIGNAL`，對端關閉時不會因 SIGPIPE 結束程式。監聽 socket 設 `SO_REUSEADDR`。

### 輸出與參數

- 移除 `pos:`、`buffer size:`、每圈的 `Ordering`；狀態只在轉換時記錄，例如 `Logging -> Ordering`。
- 每筆收發封包一行摘要；W 封包預設不逐筆顯示，`--verbose` 才顯示。
- 每隔數秒印出報價表（Symbol / Bid / Ask / Last / Chg% / Volume）與掛單數，依台股習慣漲紅跌綠。
- 參數：`--port`、`--sessions`、`--tick-ms`、`--board-sec`、`--raw`、`--verbose`、`--no-color`。

## Client 端修改（PseudoClient）

Client 拆成「接收」與「指令列」兩條執行緒，回報與報價可以在使用者輸入時即時顯示；Client 仍只做封包轉換，不做交易決策。

### 執行緒重構

- 主執行緒：執行原本的會話狀態機（Logging / Ordering / SendingLogout），只移除原本阻塞在 `std::cin` 的分支與 `is_waiting_order`。
- 指令執行緒：登入成功後（`condition_variable` 通知）開始讀取指令。輸入結束（EOF）時自動登出。
- Ordering 狀態新增 W 分支；收到 Heartbeat 不再誤觸發輸入提示。
- 使用者下 `logout` 時先進入 SendingLogout，等交易所回 5 才結束，不再重複送出兩次 Logout。
- 同樣使用 `Session::SendMsg()` 與 header 鎖；`current_state` 改為 `std::atomic`。

### 指令

| 指令 | FIX | 說明 |
| --- | --- | --- |
| `buy` / `sell <symbol> <qty> <price>` | D | 限價新單，ClOrdID 依序產生（C000001…） |
| `cancel <ClOrdID>` | F | 取消 |
| `replace <ClOrdID> <qty> <price>` | G | 改量／改價，0 表示不改 |
| `status <ClOrdID>` | H | 查詢 |
| `quotes` / `orders` | — | 顯示收到的報價／由 8、9 重建的委託表 |
| `watch` / `raw` / `verbose` `on\|off` | — | 即時報價、tag 表、除錯輸出 |
| `hb` / `test` / `logout` | 0 / 1 / 5 | 會話訊息 |
| `wait <sec>` | — | 暫停，用於腳本輸入 |

### FIX 編解碼新增（ClientMessenger）

- `MakeD/F/G/H(const OrderParams&, …)`：由使用者參數填入 11/41/55/54/38/44。封包組裝步驟與舊版 `MakeX` 相同（BodyLength = body 長度）。
- `Receive8()`、`Receive9()`、`ReceiveW()`：封包轉結構，`ReceiveW` 支援 269/270/271 重複群組。
- 送單時顯示「Order Out Register → FIX encoder」區塊：先列使用者參數，再列編碼後的封包，對應報告中的 Encoder 角色。

### 輸出與參數

- 回報非同步以顏色印出，例如 `[FILL] FILLED C000001 BUY 2330 1000 @ 602.00 cum 1000/1000`。
- 結束時印出委託總表。
- 參數：`--host`、`--port`、`--raw`、`--verbose`、`--no-color`。

## 未修改範圍與行為變更

Client 的 `ClientMessenger` 是 fix\_top 硬體模組的 Golden Reference，既有封包的位元組輸出與解碼回傳值均未改變。

### 未修改

- `PseudoServer_HFFT/` 全部。
- 既有的 `MakeA/0/1/2/2resp/3/3resp/4/5/D/F/G/H`（Client）與 Server 端對應函式。
- `SearchMsgType()` 的判斷邏輯與回傳值（0/2/3/5）；只把其中 3 行 `std::cout` 改為 `ui::Log`。`git diff` 確認 `ClientMessenger.cpp` 被刪除的行只有這 3 行。
- `TaifexORGenerator` 既有的 Serialize 函式、`TaifexOrderUtility`、`docker/`。

### 使用者可感受到的行為變更

- 舊指令 `NewOrder`、`OrderCancelRequest`、`OrderCancel`、`OrderStatus` 不再送出寫死的訂單，改為提示使用新指令（舊版 `MakeD` 的數量未初始化，交易所會一律拒絕）。`Heartbeat`、`TestRequest`、`Logout` 仍可用。
- 舊版 `OrderCancel` 指令實際送出 F、`OrderCancelRequest` 送出 G，與 `FIXMsgType` 對照表相反。新指令 `cancel` 送 F、`replace` 送 G。
- Server 以 `--sessions` 參數決定接受的連線數（預設 1，與原本相同）。

## 已知問題（本次未修正）

以下問題屬於 Golden Reference 程式碼，為避免改變硬體驗證的比對基準而保留，是否修正待決定。

| # | 問題 | 影響 | 本次的處理 |
| --- | --- | --- | --- |
| 1 | `SearchMsgType` 的 BodyLength 檢查寫成 `封包長度 - 22`，正確應為減 23 | 只有特定位數的 BodyLength 能通過；Client 送出的 D/F/G/H 在 Server 端會得到錯誤碼 3，只是 Ordering 分支忽略該錯誤 | 新的 Server 訊息以 `CompatBodyLength()` 配合此檢查 |
| 2 | CheckSum 驗證條件對整包計算且邏輯反向 | 實際上不會拒絕 CheckSum 錯誤的封包 | 未處理 |
| 3 | 舊版 `Make9` 把 `uint8_t` 的 CxlRejReason 當字元輸出 | tag 102 寫入 NUL 位元組 | 新版 `Make9` overload 改填 ASCII 數字，避開此問題 |
| 4 | Heartbeat 的 TestReqID 由 `DecodeMessage(buf, "2")` 取得 | 送出 `112=no this tag` | 未處理 |

## 驗證方式與結果

兩個專案在 docker 容器 `hfst`（C++14）中編譯無錯誤，以下 3 組情境皆以腳本輸入端到端執行通過。

| 情境 | 操作 | 結果 |
| --- | --- | --- |
| 下單流程 | 穿價買單、掛單、`replace`、`status`、`cancel`、取消不存在的訂單、未知代碼、非升降單位價格、`logout` | 依序收到 NEW→FILLED、NEW、REPLACED、STATUS、CANCELED、9 拒絕、2 筆 REJECTED，正常登出 |
| 會話維持 | TTY 模式、`raw on`、`watch on`、等待 20 秒以上、`test` | Client 心跳與 TestRequest→Heartbeat 正常，持續接收 W 期間無序號錯誤或 ResendRequest |
| 行情觸發成交 | `--tick-ms 200`，掛出買賣單後等待 8 秒 | 3 筆掛單都在後續 tick 被穿價成交 |

Golden Reference 回歸：`git diff` 確認既有 `MakeX` 與 `SearchMsgType` 的邏輯未變。

重現步驟：在容器內於兩個 `build/` 執行 `cmake .. && make`，一個終端執行 `./TaifexOrderServer`，另一個執行 `./TaifexOrderClient`。非互動測試可用：

```bash
printf "buy 2330 1000 610\nwait 2\norders\n" | ./TaifexOrderClient
```
