#ifndef __XML_IO__
#define __XML_IO__

#include <stdio.h>
#include <iostream>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <string>
#include <vector>
#include <stdexcept>
#include <memory>
#include <unordered_map>

namespace maxssau
{

    class XmlNode {
    public:
        XmlNode(xmlNodePtr node = nullptr) : node_(node) {}

        // Проверка валидности узла
        bool isValid() const { return node_ != nullptr; }

        // Получение имени узла
        std::string getName() const {
            return node_ ? reinterpret_cast<const char*>(node_->name) : "";
        }

        // Получение текстового содержимого
        std::string getText() const {
            if (!node_) return "";
            xmlChar* content = xmlNodeGetContent(node_);
            std::string result(reinterpret_cast<char*>(content));
            xmlFree(content);
            return result;
        }

        // Установка текстового содержимого
        void setText(const std::string& text) {
            if (!node_) return;
            xmlNodeSetContent(node_, BAD_CAST text.c_str());
        }

        // Получение значения атрибута
        std::string getAttribute(const std::string& name) const {
            if (!node_) return "";
            xmlChar* value = xmlGetProp(node_, BAD_CAST name.c_str());
            if (!value) return "";
            std::string result(reinterpret_cast<char*>(value));
            xmlFree(value);
            return result;
        }

        // Установка атрибута
        void setAttribute(const std::string& name, const std::string& value) {
            if (!node_) return;
            xmlSetProp(node_, BAD_CAST name.c_str(), BAD_CAST value.c_str());
        }

        // Получение количества дочерних элементов
        size_t getChildCount() const {
            if (!node_) return 0;
            size_t count = 0;
            for (xmlNodePtr child = node_->children; child; child = child->next) {
                if (child->type == XML_ELEMENT_NODE) count++;
            }
            return count;
        }

        // Получение дочернего узла по индексу
        XmlNode getChild(size_t index) const {
            if (!node_) return XmlNode();
            size_t count = 0;
            for (xmlNodePtr child = node_->children; child; child = child->next) {
                if (child->type == XML_ELEMENT_NODE) {
                    if (count++ == index) return XmlNode(child);
                }
            }
            return XmlNode();
        }

        // Получение дочернего узла по имени
        XmlNode getChild(const std::string& name) const {
            if (!node_) return XmlNode();
            for (xmlNodePtr child = node_->children; child; child = child->next) {
                if (child->type == XML_ELEMENT_NODE && 
                    xmlStrcmp(child->name, BAD_CAST name.c_str()) == 0) {
                    return XmlNode(child);
                }
            }
            return XmlNode();
        }

        // Получение всех дочерних узлов с заданным именем
        std::vector<XmlNode> getChildren(const std::string& name = "") const {
            std::vector<XmlNode> result;
            if (!node_) return result;
            
            for (xmlNodePtr child = node_->children; child; child = child->next) {
                if (child->type == XML_ELEMENT_NODE) {
                    if (name.empty() || xmlStrcmp(child->name, BAD_CAST name.c_str()) == 0) {
                        result.emplace_back(child);
                    }
                }
            }
            return result;
        }

        // Добавление дочернего узла
        XmlNode addChild(const std::string& name, const std::string& text = "") {
            if (!node_) return XmlNode();
            xmlNodePtr newChild = xmlNewChild(node_, nullptr, 
                                            BAD_CAST name.c_str(), 
                                            BAD_CAST text.c_str());
            return XmlNode(newChild);
        }

        // Удаление узла
        void remove() {
            if (node_) {
                xmlUnlinkNode(node_);
                xmlFreeNode(node_);
                node_ = nullptr;
            }
        }

        // Получение родительского узла
        XmlNode getParent() const {
            return node_ && node_->parent ? XmlNode(node_->parent) : XmlNode();
        }

        // Получение количества атрибутов
        size_t getAttributeCount() const {
            if (!node_) return 0;
            size_t count = 0;
            for (xmlAttrPtr attr = node_->properties; attr; attr = attr->next) {
                count++;
            }
            return count;
        }

        // Получение списка имен атрибутов
        std::vector<std::string> getAttributeNames() const {
            std::vector<std::string> result;
            if (!node_) return result;
            
            for (xmlAttrPtr attr = node_->properties; attr; attr = attr->next) {
                result.emplace_back(reinterpret_cast<const char*>(attr->name));
            }
            return result;
        }

        // Получение всех атрибутов в виде словаря
        std::unordered_map<std::string, std::string> getAttributes() const {
            std::unordered_map<std::string, std::string> result;
            if (!node_) return result;
            
            for (xmlAttrPtr attr = node_->properties; attr; attr = attr->next) {
                xmlChar* value = xmlGetProp(node_, attr->name);
                if (value) {
                    result[reinterpret_cast<const char*>(attr->name)] = 
                        reinterpret_cast<char*>(value);
                    xmlFree(value);
                }
            }
            return result;
        }

        // Получение указателя на низкоуровневую структуру
        xmlNodePtr raw() const { return node_; }

    private:
        xmlNodePtr node_;
    };

class XmlHandler {
public:
    // Конструктор/деструктор
    XmlHandler() : doc_(nullptr), root_(nullptr) {
        xmlInitParser();
    }
    
    ~XmlHandler() {
        close();
        xmlCleanupParser();
    }

    // Открытие XML файла
    void open(const std::string& filename) {
        close();
        doc_ = xmlReadFile(filename.c_str(), nullptr, 0);
        if (!doc_) {
            throw std::runtime_error("Failed to parse XML file: " + filename);
        }
        root_ = xmlDocGetRootElement(doc_);
    }

    // Создание нового XML документа
    void create(const std::string& rootName) {
        close();
        doc_ = xmlNewDoc(BAD_CAST "1.0");
        root_ = xmlNewNode(nullptr, BAD_CAST rootName.c_str());
        xmlDocSetRootElement(doc_, root_);
    }

    // Сохранение в файл
    void save(const std::string& filename) {
        if (!doc_) throw std::runtime_error("No document to save");
        if (xmlSaveFile(filename.c_str(), doc_) == -1) {
            throw std::runtime_error("Failed to save XML file");
        }
    }

    // Закрытие документа
    void close() {
        if (doc_) {
            xmlFreeDoc(doc_);
            doc_ = nullptr;
            root_ = nullptr;
        }
    }

    // Получение корневого элемента
    XmlNode getRoot() const {
        return XmlNode(root_);
    }

    // Класс для работы с узлами

private:
    xmlDocPtr doc_;
    xmlNodePtr root_;

    // Запрет копирования
    XmlHandler(const XmlHandler&) = delete;
    XmlHandler& operator=(const XmlHandler&) = delete;
};

}

#endif