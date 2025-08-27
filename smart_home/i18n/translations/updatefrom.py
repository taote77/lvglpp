import yaml
from pathlib import Path

def load_tr_data(tr_path):
    """加载tr.yml数据，返回{code: (ch_value, en_value)}的字典"""
    try:
        with open(tr_path, 'r', encoding='utf-8') as f:
            tr_data = yaml.safe_load(f) or []  # 处理空文件
        
        # 转换为{code: (ch, en)}的字典
        code_map = {}
        for item in tr_data:
            code = item.get('code')
            if code:
                ch = item.get('ch', '')
                en = item.get('en', '')
                code_map[code] = (ch, en)
        return code_map
    
    except yaml.YAMLError as e:
        raise ValueError(f"tr.yml解析失败: {str(e)}") from e

def update_language_file(original_path, lang_key, code_map, update_mode='non_empty'):
    """
    更新原始语言文件（如zh-CN.yml或en-US.yml）
    
    参数:
        original_path (str): 原始语言文件路径（如zh-CN.yml）
        lang_key (str): 语言键（如'zh-CN'）
        code_map (dict): {code: (ch, en)}的字典（来自tr.yml）
        update_mode (str): 更新模式
            'non_empty': 仅当tr中的值非空时更新（推荐）
            'overwrite': 强制覆盖（包括空值）
    """
    try:
        # 读取原始文件（不存在则初始化空字典）
        if Path(original_path).exists():
            with open(original_path, 'r', encoding='utf-8') as f:
                original_data = yaml.safe_load(f) or {}
        else:
            original_data = {}
        
        # 获取或初始化语言键下的字典（如zh-CN）
        lang_dict = original_data.get(lang_key, {})
        original_data[lang_key] = lang_dict  # 确保键存在
        
        # 遍历tr.yml中的code，更新对应值
        for code, (ch, en) in code_map.items():
            # 根据语言键选择要更新的值（zh-CN取ch，en-US取en）
            target_value = ch if lang_key == 'zh-CN' else en
            
            # 根据更新模式决定是否更新
            if update_mode == 'overwrite':
                lang_dict[code] = target_value
            elif update_mode == 'non_empty' and target_value.strip() != '':
                lang_dict[code] = target_value
            # 空值且模式为non_empty时不更新，保留原值
        
        # 写回原始文件（保留原有结构，无额外缩进）
        with open(original_path, 'w', encoding='utf-8') as f:
            yaml.dump(
                original_data,
                f,
                allow_unicode=True,
                sort_keys=False,  # 保持原有键顺序
                indent=2         # 与常规YAML格式一致
            )
        
        print(f"已更新 {original_path}（模式: {update_mode}）")
    
    except Exception as e:
        raise ValueError(f"更新{original_path}失败: {str(e)}") from e

def reverse_sync(tr_path, zh_path, en_path, update_mode='non_empty'):
    """主函数：将tr.yml的修改反向同步至中英文文件"""
    try:
        # 加载tr.yml数据
        code_map = load_tr_data(tr_path)
        if not code_map:
            print("tr.yml无有效数据，无需同步")
            return
        
        # 同步至中文文件（zh-CN.yml）
        update_language_file(zh_path, 'zh-CN', code_map, update_mode)
        
        # 同步至英文文件（en-US.yml）
        update_language_file(en_path, 'en-US', code_map, update_mode)
        
        print("反向同步完成！")
    
    except Exception as e:
        print(f"反向同步失败: {str(e)}")

if __name__ == "__main__":
    # 配置路径（根据实际情况修改）
    TR_PATH = "pro-i18n.yml"         # 合并后的tr.yml路径
    ZH_PATH = "zh-CN.yml"      # 原始中文YAML路径
    EN_PATH = "en-US.yml"      # 原始英文YAML路径
    
    # 执行反向同步（update_mode可选'non_empty'或'overwrite'）
    reverse_sync(
        tr_path=TR_PATH,
        zh_path=ZH_PATH,
        en_path=EN_PATH,
        update_mode='overwrite'  # 仅更新非空值（推荐）
    )
    