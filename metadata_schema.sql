-- DROP SCHEMA metadata;

CREATE SCHEMA metadata AUTHORIZATION ml_role;

-- DROP TYPE metadata.data_registry;

CREATE TYPE metadata.data_registry AS (
	id bigserial,
	index_type_id int4,
	field_type_id int4,
	field_description text,
	uri text);

-- DROP TYPE metadata.data_registry_id_seq;

CREATE TYPE metadata.data_registry_id_seq AS (
	"last_value" int8,
	log_cnt int8,
	is_called bool);

-- DROP TYPE metadata.feed_registry;

CREATE TYPE metadata.feed_registry AS (
	id serial4,
	uri text);

-- DROP TYPE metadata.feed_registry_id_seq;

CREATE TYPE metadata.feed_registry_id_seq AS (
	"last_value" int8,
	log_cnt int8,
	is_called bool);

-- DROP TYPE metadata.human_metadata;

CREATE TYPE metadata.human_metadata AS (
	pending bool,
	field_description text,
	"type" text,
	data_uri text,
	feed_uri text,
	feed_args text,
	source_uri text,
	source_args text);

-- DROP TYPE metadata.index_type_id_seq;

CREATE TYPE metadata.index_type_id_seq AS (
	"last_value" int8,
	log_cnt int8,
	is_called bool);

-- DROP TYPE metadata.metadata_view;

CREATE TYPE metadata.metadata_view AS (
	source_id int4,
	source_uri text,
	source_args text,
	feed_uri text,
	feed_args text,
	data_uri text,
	pending bool);

-- DROP TYPE metadata.source_binding;

CREATE TYPE metadata.source_binding AS (
	id bigserial,
	data_id int4,
	source_id int4,
	feed_args text);

-- DROP TYPE metadata.source_binding_id_seq;

CREATE TYPE metadata.source_binding_id_seq AS (
	"last_value" int8,
	log_cnt int8,
	is_called bool);

-- DROP TYPE metadata.source_registry;

CREATE TYPE metadata.source_registry AS (
	id serial4,
	uri text,
	args text,
	pending bool,
	feed_id int4);

-- DROP TYPE metadata.source_registry_id_seq;

CREATE TYPE metadata.source_registry_id_seq AS (
	"last_value" int8,
	log_cnt int8,
	is_called bool);

-- DROP TYPE metadata.type_registry;

CREATE TYPE metadata.type_registry AS (
	id int4,
	"name" text,
	description text);

-- DROP TYPE metadata."_data_registry";

CREATE TYPE metadata."_data_registry" (
	INPUT = array_in,
	OUTPUT = array_out,
	RECEIVE = array_recv,
	SEND = array_send,
	ANALYZE = array_typanalyze,
	ALIGNMENT = 8,
	STORAGE = any,
	CATEGORY = A,
	ELEMENT = metadata.data_registry,
	DELIMITER = ',');

-- DROP TYPE metadata."_feed_registry";

CREATE TYPE metadata."_feed_registry" (
	INPUT = array_in,
	OUTPUT = array_out,
	RECEIVE = array_recv,
	SEND = array_send,
	ANALYZE = array_typanalyze,
	ALIGNMENT = 8,
	STORAGE = any,
	CATEGORY = A,
	ELEMENT = metadata.feed_registry,
	DELIMITER = ',');

-- DROP TYPE metadata."_human_metadata";

CREATE TYPE metadata."_human_metadata" (
	INPUT = array_in,
	OUTPUT = array_out,
	RECEIVE = array_recv,
	SEND = array_send,
	ANALYZE = array_typanalyze,
	ALIGNMENT = 8,
	STORAGE = any,
	CATEGORY = A,
	ELEMENT = metadata.human_metadata,
	DELIMITER = ',');

-- DROP TYPE metadata."_metadata_view";

CREATE TYPE metadata."_metadata_view" (
	INPUT = array_in,
	OUTPUT = array_out,
	RECEIVE = array_recv,
	SEND = array_send,
	ANALYZE = array_typanalyze,
	ALIGNMENT = 8,
	STORAGE = any,
	CATEGORY = A,
	ELEMENT = metadata.metadata_view,
	DELIMITER = ',');

-- DROP TYPE metadata."_source_binding";

CREATE TYPE metadata."_source_binding" (
	INPUT = array_in,
	OUTPUT = array_out,
	RECEIVE = array_recv,
	SEND = array_send,
	ANALYZE = array_typanalyze,
	ALIGNMENT = 8,
	STORAGE = any,
	CATEGORY = A,
	ELEMENT = metadata.source_binding,
	DELIMITER = ',');

-- DROP TYPE metadata."_source_registry";

CREATE TYPE metadata."_source_registry" (
	INPUT = array_in,
	OUTPUT = array_out,
	RECEIVE = array_recv,
	SEND = array_send,
	ANALYZE = array_typanalyze,
	ALIGNMENT = 8,
	STORAGE = any,
	CATEGORY = A,
	ELEMENT = metadata.source_registry,
	DELIMITER = ',');

-- DROP TYPE metadata."_type_registry";

CREATE TYPE metadata."_type_registry" (
	INPUT = array_in,
	OUTPUT = array_out,
	RECEIVE = array_recv,
	SEND = array_send,
	ANALYZE = array_typanalyze,
	ALIGNMENT = 8,
	STORAGE = any,
	CATEGORY = A,
	ELEMENT = metadata.type_registry,
	DELIMITER = ',');

-- DROP SEQUENCE metadata.data_registry_id_seq;

CREATE SEQUENCE metadata.data_registry_id_seq
	INCREMENT BY 1
	MINVALUE 1
	MAXVALUE 9223372036854775807
	START 1
	CACHE 1
	NO CYCLE;
-- DROP SEQUENCE metadata.feed_registry_id_seq;

CREATE SEQUENCE metadata.feed_registry_id_seq
	INCREMENT BY 1
	MINVALUE 1
	MAXVALUE 2147483647
	START 1
	CACHE 1
	NO CYCLE;
-- DROP SEQUENCE metadata.index_type_id_seq;

CREATE SEQUENCE metadata.index_type_id_seq
	INCREMENT BY 1
	MINVALUE 1
	MAXVALUE 2147483647
	START 1
	CACHE 1
	NO CYCLE;
-- DROP SEQUENCE metadata.source_binding_id_seq;

CREATE SEQUENCE metadata.source_binding_id_seq
	INCREMENT BY 1
	MINVALUE 1
	MAXVALUE 9223372036854775807
	START 1
	CACHE 1
	NO CYCLE;
-- DROP SEQUENCE metadata.source_registry_id_seq;

CREATE SEQUENCE metadata.source_registry_id_seq
	INCREMENT BY 1
	MINVALUE 1
	MAXVALUE 2147483647
	START 1
	CACHE 1
	NO CYCLE;-- metadata.data_registry definition

-- Drop table

-- DROP TABLE metadata.data_registry;

CREATE TABLE metadata.data_registry (
	id bigserial NOT NULL,
	index_type_id int4 NOT NULL,
	field_type_id int4 NOT NULL,
	field_description text NULL,
	uri text NULL
);
CREATE UNIQUE INDEX data_registry_id_idx ON metadata.data_registry USING btree (id);


-- metadata.feed_registry definition

-- Drop table

-- DROP TABLE metadata.feed_registry;

CREATE TABLE metadata.feed_registry (
	id serial4 NOT NULL,
	uri text NOT NULL
);
CREATE UNIQUE INDEX feed_registry_id_idx ON metadata.feed_registry USING btree (id);


-- metadata.source_binding definition

-- Drop table

-- DROP TABLE metadata.source_binding;

CREATE TABLE metadata.source_binding (
	id bigserial NOT NULL,
	data_id int4 NOT NULL,
	source_id int4 NOT NULL,
	feed_args text NULL
);
CREATE UNIQUE INDEX source_binding_id_idx ON metadata.source_binding USING btree (id);


-- metadata.source_registry definition

-- Drop table

-- DROP TABLE metadata.source_registry;

CREATE TABLE metadata.source_registry (
	id serial4 NOT NULL,
	uri text NOT NULL,
	args text NULL,
	pending bool NOT NULL DEFAULT false,
	feed_id int4 NULL
);
CREATE UNIQUE INDEX source_registry_id_idx ON metadata.source_registry USING btree (id);


-- metadata.type_registry definition

-- Drop table

-- DROP TABLE metadata.type_registry;

CREATE TABLE metadata.type_registry (
	id int4 NOT NULL DEFAULT nextval('metadata.index_type_id_seq'::regclass),
	"name" text NOT NULL,
	description text NULL
);


-- metadata.human_metadata source

CREATE OR REPLACE VIEW metadata.human_metadata
AS SELECT sr.pending,
    dr.field_description,
    tr.name AS type,
    dr.uri AS data_uri,
    fr.uri AS feed_uri,
    sb.feed_args,
    sr.uri AS source_uri,
    sr.args AS source_args
   FROM metadata.source_binding sb
     JOIN metadata.data_registry dr ON sb.data_id = dr.id
     JOIN metadata.source_registry sr ON sb.source_id = sr.id
     JOIN metadata.feed_registry fr ON sr.feed_id = fr.id
     JOIN metadata.type_registry tr ON dr.field_type_id = tr.id
  ORDER BY dr.uri;


-- metadata.metadata_view source

CREATE OR REPLACE VIEW metadata.metadata_view
AS SELECT sr.id AS source_id,
    sr.uri AS source_uri,
    sr.args AS source_args,
    fr.uri AS feed_uri,
    sb.feed_args,
    dr.uri AS data_uri,
    sr.pending
   FROM metadata.source_binding sb
     JOIN metadata.data_registry dr ON sb.data_id = dr.id
     JOIN metadata.source_registry sr ON sb.source_id = sr.id
     JOIN metadata.feed_registry fr ON sr.feed_id = fr.id
  ORDER BY dr.uri;
