// Standardize
for (auto& point : data_) {
for (size_t i = 0; i < numFeatures; ++i) {
if (stdDevs[i] != 0) {
 point[i] = (point[i] - means[i]) / stdDevs[i];
 }
 }
   }
}

void Dataset::normalize() {
if (data_.empty()) return;
    size_t numFeatures = data_[0].size();
    Point mins = data_[0];
    Point maxs = data_[0];

    // Find min and max for each feature
    for (const auto& point : data_) {
 for (size_t i = 0; i < numFeatures; ++i) {
 if (point[i] < mins[i]) mins[i] = point[i];
  if (point[i] > maxs[i]) maxs[i] = point[i];
  }
    }

    // Normalize
    for (auto& point : data_) {
for (size_t i = 0; i < numFeatures; ++i) {
 if (maxs[i] - mins[i] != 0) {
   point[i] = (point[i] - mins[i]) / (maxs[i] - mins[i]);
  }
 }
    }
}

} // namespace meanshift