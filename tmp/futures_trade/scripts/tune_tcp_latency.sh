echo "1" | sudo tee /proc/sys/net/ipv4/tcp_low_latency

echo "0" | sudo tee /proc/sys/net/ipv4/tcp_slow_start_after_idle
