function [normalized, dataMeans, dataStds] = normalizeData(data)
	range = max(data) - min(data);
	scaled = data - mean(data);
	normalized = scaled ./ range;
	dataMeans = mean(normalized);
	dataStds = std(normalized);
end