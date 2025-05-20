# RobotAGV

Một triển khai mô-đun và hướng đối tượng của robot AGV (Automated Guided Vehicle) sử dụng ESP32.

![RobotAGV](docs/images/robot.jpg)

## Tổng Quan Dự Án

Repository này chứa toàn bộ mã nguồn của robot AGV dựa trên nền tảng ESP32. Dự án được cấu trúc theo nguyên tắc lập trình hướng đối tượng để dễ bảo trì và mở rộng.

### Tính Năng Chính

- **Điều hướng theo phương pháp dạy và thực hiện**: Dạy robot các đường đi thông qua ứng dụng di động mà không cần dùng dò line
- **Điều khiển PID-Backstepping**: Thuật toán điều khiển tiên tiến cho việc bám quỹ đạo chính xác
- **Điều khiển từ xa qua WiFi**: Điều khiển robot thông qua ứng dụng di động hoặc giao diện web
- **Theo dõi vị trí**: Đo lường chính xác vị trí sử dụng bộ mã hóa bánh xe
- **Kiến trúc mô-đun**: Cấu trúc mã nguồn hướng đối tượng được tổ chức tốt

## Cấu Trúc Dự Án

Mã nguồn được tổ chức thành nhiều mô-đun:

- **RobotAGV.ino**: File sketch Arduino chính khởi tạo và điều phối tất cả các thành phần
- **Config.h**: File cấu hình trung tâm với tất cả các tham số và định nghĩa chân
- **Motor.h**: Lớp điều khiển động cơ với chức năng PWM
- **Encoder.h/Encoder.cpp**: Quản lý bộ mã hóa và xử lý ngắt
- **PIDController.h**: Triển khai thuật toán điều khiển PID
- **Robot.h**: Lớp robot chính xử lý động học, theo dõi vị trí và bám quỹ đạo
- **WebServer.h**: Giao diện web để điều khiển từ xa qua WiFi

## Chi Tiết Kỹ Thuật

### Động Học Robot
- Robot sử dụng động học vi sai (differential drive)
- Theo dõi vị trí sử dụng bộ mã hóa bánh xe (x, y, theta)
- Bám quỹ đạo với hiệu chỉnh lỗi trong hệ tọa độ của robot

### Hệ Thống Điều Khiển
- Bộ điều khiển PID cho mỗi động cơ
- Thuật toán bám quỹ đạo với giảm thiểu lỗi vị trí và hướng
- Triển khai chống tích lũy (anti-windup) cho điều khiển tích phân

### Truyền Thông
- Điểm truy cập WiFi để kết nối trực tiếp
- API HTTP đơn giản cho các lệnh điều khiển
- Giao diện ứng dụng di động để dạy quỹ đạo

## Yêu Cầu Phần Cứng

- Board phát triển ESP32
- Hai động cơ DC với bộ mã hóa (234.3 xung/vòng)
- Driver động cơ H-bridge
- Màn hình LCD I2C (16x2)
- Nút nhấn để điều khiển vật lý
- Nguồn điện

## Thiết Kế PCB

Thư mục `pcb` chứa các file thiết kế PCB Altium cho phần cứng điện tử của robot. PCB tích hợp:
- Vi điều khiển ESP32
- Mạch điều khiển động cơ
- Giao diện bộ mã hóa
- Quản lý nguồn điện
- Kết nối I/O

## Ứng Dụng Di Động

Thư mục `app` chứa ứng dụng di động được thiết kế với MIT App Inventor cho phép:
- Dạy robot các đường đi mới
- Điều khiển từ xa chuyển động của robot
- Lưu trữ và quản lý nhiều quỹ đạo
- Giám sát trạng thái robot

## Hướng Dẫn Sử Dụng

### Thiết Lập
1. Tải mã nguồn lên board ESP32 với các kết nối phần cứng cần thiết
2. Bật nguồn robot
3. Kết nối với điểm truy cập WiFi (SSID: ESP32-AP, Mật khẩu: 123456789)

### Điều Khiển Từ Xa
Điều khiển robot bằng cách gửi yêu cầu HTTP đến:
- `/T`: Di chuyển tiến
- `/B`: Di chuyển lùi
- `/L`: Rẽ trái
- `/R`: Rẽ phải
- `/0`: Dừng lại
- `/1`: Bắt đầu bám quỹ đạo
- `/2`: Dừng bám quỹ đạo
- `/3`: Bắt đầu ghi quỹ đạo
- `/4`: Dừng ghi quỹ đạo
- `/5`: Xóa quỹ đạo đã ghi
- `/6`: Tiếp tục bám quỹ đạo

### Điều Khiển Vật Lý
- **Nút Start**: Bắt đầu bám quỹ đạo
- **Nút Stop**: Dừng khẩn cấp

## Chi Tiết Triển Khai

### Hệ Tọa Độ
- X: Vị trí tiến/lùi tính bằng mét
- Y: Vị trí trái/phải tính bằng mét
- Theta: Hướng tính bằng radian

### Bám Quỹ Đạo
Robot có thể ghi lại quỹ đạo khi bạn điều khiển thủ công, sau đó thực hiện lại quỹ đạo đó bằng thuật toán điều khiển giảm thiểu lỗi vị trí và hướng.

### Tính Năng Chính
1. **Dạy qua Ứng dụng Di động**: Robot có thể được dạy các đường đi thông qua ứng dụng di động mà không cần dùng đường dò line dán dưới sàn.
2. **Thuật toán PID-Backstepping**: Sử dụng điều khiển tiên tiến để bám quỹ đạo chính xác.
3. **Tự động Hiệu chỉnh Đường đi**: Nếu bị đẩy ra khỏi quỹ đạo, robot tự động quay lại đúng đường đã lập trình.
4. **Lưu trữ Vị trí**: Lưu dữ liệu vị trí để hoạt động nhất quán.
5. **Phát hiện Vật cản**: Tự động dừng lại khi phát hiện vật cản.

## Cài Đặt

1. Clone repository này
2. Mở file `src/RobotAGV.ino` trong Arduino IDE
3. Cài đặt các thư viện cần thiết:
   - WiFi
   - WebServer
   - MatrixMath
   - Wire
   - LiquidCrystal_I2C
4. Kết nối board ESP32
5. Tải sketch lên board

## Giấy Phép

Dự án này là mã nguồn mở và có sẵn dưới Giấy phép MIT.

## Người Đóng Góp

- [Tên của bạn](https://github.com/tên-người-dùng-của-bạn)

## Lời Cảm Ơn

- Cảm ơn đặc biệt đến [đề cập bất kỳ người đóng góp hoặc nguồn tài nguyên đã giúp đỡ]
