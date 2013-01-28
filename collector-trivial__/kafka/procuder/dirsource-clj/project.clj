(defproject kafka-dir-source "1.0.0-SNAPSHOT"
  :description "FIXME: write description"
  :dependencies [
                [log4j/log4j "1.2.16"]
                [org.clojure/tools.logging "0.2.3"]
                [org.clojure/tools.cli "0.2.1"]
                [org.clojure/clojure "1.4.0"]
                [org.clojure/java.jdbc "0.2.0"]
                [org.xerial/sqlite-jdbc "3.7.2"]
                [com.netflix.curator/curator-framework "1.0.1"]
                [com.googlecode.disruptor/disruptor "2.10"]
                [kafka-clj-cli/kafka-clj-cli "0.1-SNAPSHOT"]
                ]
  :dev-dependencies [
                [lein-eclipse "1.0.0"]
                ]
  :aot :all
  )
