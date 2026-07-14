# Space Invaders - STM32F429I-DISCO
## Giới thiệu
Dự án Game Space Invaders (Bắn Ruồi) đồ họa 2D chạy trên board mạch phát triển STM32F429I-DISCO. Game được phát triển hoàn toàn bằng C++ và sử dụng framework đồ họa **TouchGFX** kết hợp thư viện dựng hình tự chế `RetroGraphics` để tối ưu hóa hiệu năng vẽ màn hình LTDC với bộ gia tốc phần cứng DMA2D. 
## Cấu trúc thư mục (Directory Structure)
- `Core/`: Chứa mã nguồn cấu hình phần cứng cơ bản của vi điều khiển (HAL, System Clock, I2C, SPI, DMA2D, LTDC, FMC...).
    - `Src/main.c`: Tệp chương trình chính, nơi khởi tạo vi điều khiển và bắt đầu luồng của hệ điều hành FreeRTOS.
    - `Src/sysmem.c`: Xử lý phân bổ bộ nhớ cho thư viện chuẩn C.
- `TouchGFX/`: Nơi chứa toàn bộ tài nguyên và mã nguồn logic của Game.
    - `assets/`: Hình ảnh, font chữ gốc `.png` dùng trong game.
    - `gui/`: Mã nguồn logic của game.
        - `src/screen1_screen/GameLogic.cpp`: Chứa toàn bộ các xử lý logic về vật lý, sinh quái, đạn bay, hiệu ứng nổ, va chạm, tính điểm và độ khó.
        - `src/screen1_screen/GameRenderer.cpp`: Quản lý việc render hình ảnh, ảnh động, và text lên màn hình ILI9341.
        - `src/screen1_screen/Screen1View.cpp`: Chứa luồng cập nhật frame từ TouchGFX.
        - `include/gui/screen1_screen/RetroGraphics.hpp`: Bộ thư viện đồ họa tối giản tự viết cho phép vẽ các pixel thô, font chữ máy thùng (retro arcade 7x7) và pha trộn ảnh Bitmap hiệu năng cao nhằm bỏ qua giới hạn của Canvas TouchGFX.
    - `generated/`: Chứa các tài nguyên ảnh (RGB565, ARGB8888) đã được công cụ TouchGFX Designer tự động biên dịch thành các tệp `.cpp`.
- `STM32CubeIDE/`: Nơi lưu trữ các tệp cấu hình cho môi trường phát triển và quá trình biên dịch GCC.
## Sơ đồ cấu hình chân (Pin Configuration & Hardware Setup)
Dự án chạy trực tiếp trên vi điều khiển STM32F429ZIT6. Các ngoại vi đồ họa cần kết nối nội bộ thông qua các bus rất phức tạp để vận hành màn hình:
1. **Màn hình TFT LCD (ILI9341 - 240x320, RGB565):**
    - **LTDC (LCD-TFT Display Controller):** Giao tiếp đồ họa chính, sử dụng lên tới 22 chân được map chằng chịt trên các port A, B, C, D, F, G để liên tục đẩy tín hiệu màu RGB thô (R: 5 bit, G: 6 bit, B: 5 bit), HSYNC, VSYNC, DE và Clock (DOTCLK).
    - **SPI5 (PF7, PF8, PF9):** Được dùng một lần lúc chip vừa khởi động để cấu hình thanh ghi cho IC ILI9341 (Đánh thức màn hình, Lật chiều dọc/ngang, Chỉnh định dạng pixel).
2. **SDRAM ngoài (IS42S16400J - 64Mbit):**
    - **FMC (Flexible Memory Controller):** Cần dùng SDRAM vì RAM nội bộ của vi điều khiển (~256KB) không đủ sức chứa 2 bộ đệm khung hình (Framebuffer) liên tục cho màn hình (240x320x2 bytes x 2 buffers = ~307KB).
    - Các tín hiệu Address, Data (16 bit), Clock, NWE... chiếm dụng gần như tất cả các chân còn trống ở Port B, C, D, E, F, G.
3. **Nút bấm người dùng (User Button):**
    - Được nối cứng vào chân **`PA0`** (Nút màu xanh lam trên kit). Dùng để điều khiển tàu vũ trụ và bắn đạn.
## Luồng hoạt động (Operating Flow)
1. **Khởi động phần cứng:** Vi điều khiển được cấp điện, chạy `main()` -> Gọi các hàm khởi tạo từ STM32CubeMX: cấu hình Xung nhịp (Clock Tree), bật FMC cho SDRAM, bật cấu hình LTDC, DMA2D.
2. **Boot hệ điều hành:** Gọi thư viện `touchgfx_init()` để bắt đầu engine đồ họa và cấp phát Framebuffer trên vùng nhớ SDRAM ngoài (`0xD0000000`). Khởi tạo luồng (Task) đồ họa và chuyển quyền CPU cho **FreeRTOS**.
3. **Vòng lặp Game (Game Loop - 60 FPS):**
    - Màn hình liên tục đẩy tín hiệu đồng bộ dọc (VSYNC) cho vi điều khiển, báo hiệu đã vẽ xong 1 frame. Hệ điều hành phản hồi bằng cách gọi hàm `handleTickEvent()` của View.
    - **Update Data:** TouchGFX gọi vào hàm `GameLogic::tick()`. Lúc này, hệ thống sẽ di chuyển phi thuyền, đẩy các viên đạn bay lên/xuống, kiểm tra xem có viên đạn nào đập vào quái vật hay Boss không, và xử lý hiệu ứng bùm chéo.
    - **Render:** TouchGFX gọi `GameRenderer::draw()`. Tùy thuộc vào tọa độ vật lý mới, thuật toán đồ họa `RetroGraphics` sẽ ra lệnh vẽ các vật thể. Quan trọng: engine chỉ tính toán trên các pixel thuộc vùng bị thay đổi ảnh (`invalidatedArea`) nhằm giảm tải tối đa cho GPU.
    - **Hardware Acceleration:** Bộ gia tốc phần cứng **DMA2D** nhận lệnh và tự động trộn pixel màu (Alpha blending) từ Flash ROM vào SDRAM mà không bắt CPU phải làm việc.
4. **Xử lý sự kiện (Input):** Khi nút bấm vật lý (PA0) được nhấn hoặc màn hình cảm ứng được chạm, `GameCanvas::handleKeyEvent()` sẽ kích hoạt phi thuyền xả đạn!
## Cài đặt & Chạy dự án (Installation & Running)
### 1. Yêu cầu phần mềm
- Cài đặt **STM32CubeIDE** (phiên bản 1.14 trở lên).
- Cài đặt **TouchGFX Designer** (phiên bản 4.24.x tương ứng với cấu hình project).
### 2. Các bước cài đặt và chạy
1. **Tạo tài nguyên đồ họa:** 
   - Điều hướng tới thư mục `TouchGFX/`.
   - Click đúp để mở file `SpaceInvaders.touchgfx` bằng TouchGFX Designer.
   - Nhấn nút **Generate Code** (hoặc phím F4) để phần mềm xử lý hình ảnh `.png` thành mảng byte hex trong tệp mã nguồn `.cpp`. Khi quá trình thành công, đóng phần mềm TouchGFX lại.
2. **Mở dự án bằng IDE:** 
   - Khởi động **STM32CubeIDE**.
   - Mở dự án qua đường dẫn: `File -> Open Projects from File System...` -> Chọn thư mục gốc `SpaceInvaders` -> Nhấn *Finish*.
3. **Biên dịch (Build):** 
   - Nhấn vào biểu tượng cái búa 🔨 (**Build**) trên thanh công cụ trên cùng. Quá trình này sẽ dịch mã nguồn đồ họa và nhân hệ điều hành thành tệp thực thi. Đảm bảo Console in ra `Build Finished. 0 errors`.
4. **Nạp Firmware (Flash/Run):** 
   - Cắm cáp kết nối cổng **mini-USB** (cổng phía trên, gần chữ ST-LINK) vào máy tính.
   - Nhấn vào nút hình mũi tên màu xanh 🟢 (**Run** hoặc **Debug**). STM32CubeIDE sẽ kết nối ST-Link và nạp thẳng game xuống vi điều khiển.
5. **Chơi Game!** 
   - Khi chip khởi động xong, game sẽ hiển thị. Nhấn mạnh nút **USER** (màu xanh lam) trên board mạch để Start và nã đạn bảo vệ hệ mặt trời!
> **Lưu ý khắc phục lỗi:** Nếu bạn nhấn *Debug* mà gặp lỗi `Setup exceptions` báo đỏ, hãy thử rút cáp USB cắm lại, hoặc dùng phần mềm *STM32CubeProgrammer* nhấn nút *Full Chip Erase* để xóa sạch firmware cũ gây treo chip rồi nạp lại.
