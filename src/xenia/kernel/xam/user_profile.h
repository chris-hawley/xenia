
        length = 0;
        xe::store_and_swap<int32_t>(user_data + kValueOffset, 0);
        xe::store_and_swap<uint32_t>(user_data + kPointerOffset, 0);
      } else {
        length = 2 * (static_cast<int32_t>(value.size()) + 1);
        xe::store_and_swap<int32_t>(user_data + kValueOffset, length);
        xe::store_and_swap<uint32_t>(
            user_data + kPointerOffset,
            buffer_ptr + static_cast<uint32_t>(buffer_offset));
        memcpy(buffer + buffer_offset, value.data(), length);
      }
      return buffer_offset + length;
    }
  };
  struct FloatSetting : public Setting {
    FloatSetting(uint32_t setting_id, float value)
        : Setting(setting_id, Type::FLOAT, 4, true), value(value) {}
    float value;
    size_t Append(uint8_t* user_data, uint8_t* buffer, uint32_t buffer_ptr,
                  size_t buffer_offset) override {
      buffer_offset =
          Setting::Append(user_data, buffer, buffer_ptr, buffer_offset);
      xe::store_and_swap<float>(user_data + kValueOffset, value);
      return buffer_offset;
    }
  };
  struct BinarySetting : public Setting {
    BinarySetting(uint32_t setting_id)
        : Setting(setting_id, Type::BINARY, 8, false), value() {}
    BinarySetting(uint32_t setting_id, const std::vector<uint8_t>& value)
        : Setting(setting_id, Type::BINARY, 8, true), value(value) {}
    std::vector<uint8_t> value;
    size_t extra_size() const override {
      return static_cast<int32_t>(value.size());
    }
    size_t Append(uint8_t* user_data, uint8_t* buffer, uint32_t buffer_ptr,
                  size_t buffer_offset) override {
      buffer_offset =
          Setting::Append(user_data, buffer, buffer_ptr, buffer_offset);
      int32_t length;
      if (value.empty()) {
        length = 0;
        xe::store_and_swap<int32_t>(user_data + kValueOffset, 0);
        xe::store_and_swap<int32_t>(user_data + kPointerOffset, 0);
      } else {
        length = static_cast<int32_t>(value.size());
        xe::store_and_swap<int32_t>(user_data + kValueOffset, length);
        xe::store_and_swap<uint32_t>(
            user_data + kPointerOffset,
            buffer_ptr + static_cast<uint32_t>(buffer_offset));
        memcpy(buffer + buffer_offset, value.data(), length);
      }
      return buffer_offset + length;
    }
    std::vector<uint8_t> Serialize() const override {
      return std::vector<uint8_t>(value.data(), value.data() + value.size());
    }
    void Deserialize(std::vector<uint8_t> data) override {
      value = data;
      is_set = true;
    }
  };
  struct DateTimeSetting : public Setting {
    DateTimeSetting(uint32_t setting_id, int64_t value)
        : Setting(setting_id, Type::DATETIME, 8, true), value(value) {}
    int64_t value;
    size_t Append(uint8_t* user_data, uint8_t* buffer, uint32_t buffer_ptr,
                  size_t buffer_offset) override {
      buffer_offset =
          Setting::Append(user_data, buffer, buffer_ptr, buffer_offset);
      xe::store_and_swap<int64_t>(user_data + kValueOffset, value);
      return buffer_offset;
    }
  };

  static bool DecryptAccountFile(const uint8_t* data, X_XAMACCOUNTINFO* output,
                                 bool devkit = false);

  static void EncryptAccountFile(const X_XAMACCOUNTINFO* input, uint8_t* output,
                                 bool devkit = false);

  UserProfile();

  uint64_t xuid() const { return account_.xuid_online; }
  std::string name() const { return account_.GetGamertagString(); }
  uint32_t signin_state() const { return 1; }

  void AddSetting(std::unique_ptr<Setting> setting);
  Setting* GetSetting(uint32_t setting_id);

  xdbf::GpdFile* SetTitleSpaData(const xdbf::SpaFile& spa_data);
  xdbf::GpdFile* GetTitleGpd(uint32_t title_id = -1);

  void GetTitles(std::vector<xdbf::GpdFile*>& titles);

  bool UpdateTitleGpd(uint32_t title_id = -1);
  bool UpdateAllGpds();

 private:
  void LoadProfile();
  bool UpdateGpd(uint32_t title_id, xdbf::GpdFile& gpd_data);

  X_XAMACCOUNTINFO account_;
  std::vector<std::unique_ptr<Setting>> setting_list_;
  std::unordered_map<uint32_t, Setting*> settings_;

  std::unordered_map<uint32_t, xdbf::GpdFile> title_gpds_;
  xdbf::GpdFile dash_gpd_;
  xdbf::GpdFile* curr_gpd_ = nullptr;
  uint32_t curr_title_id_ = -1;
};

}  // namespace xam
}  // namespace kernel
}  // namespace xe

#endif  // XENIA_KERNEL_XAM_USER_PROFILE_H_
