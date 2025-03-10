# Robot AGV Vận Chuyển Hàng Trong Nhà Máy 
## Nội dung
1. Kết Nối App Điện Thoại Để Dạy Đường Đi Cho Robot
- App được thiết kế bằng MIT App Inventor  
- Robot có thể được điều khiển và dạy đường đi thông qua ứng dụng điện thoại mà không cần sử dụng các đường dò line dán dưới sàn.  
- Ban đầu, khi robot được đưa vào nhà máy, người dùng sẽ dạy cho robot biết các vị trí cần đi qua để thu gom rác. Điều này giúp robot dễ dàng thích nghi và linh hoạt với môi trường làm việc trong nhà máy.  
- Khi thay đổi vị trí của dây chuyền hoặc khu vực làm việc, chỉ cần cập nhật lại lộ trình mà không cần thay đổi cấu trúc cơ bản của hệ thống.  
2. Sử Dụng Thuật Toán PID-Backstepping Để Robot Bám Quy Đạo  
- Robot sử dụng thuật toán PID-Backstepping  để bám sát quỹ đạo đã được dạy từ trước.  
- Với thuật toán trên, robot có thể đi theo đường đã học mà không cần dùng đường line gắn dưới sàn.  
- Một tính năng đặc biệt là robot có thể tự động điều chỉnh lại quỹ đạo nếu bị tác động bởi ngoại lực (ví dụ: bị lệch hướng do người vứt rác hoặc các yếu tố khác). Robot sẽ tự động quay lại đúng quỹ đạo đã lưu trữ.  
4. Lưu Vị Trí Của Robot Bằng Thẻ Nhớ  
- Robot sử dụng module thẻ nhớ để lưu trữ thông tin về vị trí hiện tại và quá trình di chuyển, giúp theo dõi và kiểm soát chính xác đường đi của robot.  
5. Tự Động Xác Định Vật Cản Và Dừng Lại  
- Robot có khả năng tự động nhận diện vật cản trên đường di chuyển. Khi gặp vật cản, robot sẽ dừng lại và phát âm thanh thông báo, giúp đảm bảo an toàn và hiệu quả trong quá trình vận hành.  
![image](https://github.com/user-attachments/assets/640a406c-cf21-4dec-b8d9-9a77d24a4aab)
