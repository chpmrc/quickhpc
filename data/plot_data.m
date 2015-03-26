clear spy_data_orig;
clear victim_data_orig;

spy_data_orig = csvread('spy_data');
victim_data_orig = csvread('victim_data');

spy_data = zeros(size(spy_data_orig, 1), size(spy_data_orig, 2));
victim_data = zeros(size(victim_data_orig, 1), size(victim_data_orig, 2));

spy_mean = 0;
victim_mean = 0;
spy_std = 0;
victim_std = 0;

[spy_data, spy_mean, spy_std] = normalizeData(spy_data_orig);
[victim_data, victim_mean, victim_std] = normalizeData(victim_data_orig);

% figure(1);
% hold on;
% xlabel('CPU Instructions');
% ylabel('Total L3 cache misses and accesses');
% plot(spy_data(:, 1), spy_data(:, 2), '*', 'Color', [1, 0, 0]);
% plot(spy_data(:, 1), spy_data(:, 3), '*', 'Color', [0, 1, 0]);
% print -djpeg spy_plot.jpg

% figure(2);
% hold on;
% plot(victim_data(:, 1), victim_data(:, 2), '*', 'Color', [0, 0, 1]);
% plot(victim_data(:, 1), victim_data(:, 3), '*', 'Color', [1, 0, 1]);
% legend('Spy L3 cache misses', 'Spy L3 cache accesses', 'Victim L3 cache misses', 'Victim L3 cache accesses');
% print -djpeg victim_plot.jpg

figure(3);
hold on;
title('Correlation between spy and victim', 'FontSize', 15);
plot(spy_data(:, 2), 'r*');
plot(victim_data(:, 2), 'b*');
xlabel('Samples')
ylabel('Total L3 cache accesses');
legend('Yarom spy L3 accesses', 'Openssl L3 accesses');
print -djpeg correlation.jpg