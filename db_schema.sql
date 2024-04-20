-- DROP SCHEMA metadata;

CREATE SCHEMA metadata AUTHORIZATION ml_role;

-- DROP TYPE metadata.data_registry;

CREATE TYPE metadata.data_registry AS (
	id bigserial,
	index_type_id int4,
	field_type_id int4,
	field_description text,
	uri text);

-- DROP TYPE metadata.feature;

CREATE TYPE metadata.feature AS (
	id int8,
	"label" text,
	formula json);

-- DROP TYPE metadata.feed_registry;

CREATE TYPE metadata.feed_registry AS (
	id serial4,
	uri text);

-- DROP TYPE metadata.human_metadata_view;

CREATE TYPE metadata.human_metadata_view AS (
	id int8,
	"always" bool,
	pending bool,
	field_description text,
	"type" text,
	data_uri text,
	feed_uri text,
	feed_args text,
	source_uri text,
	source_args text);

-- DROP TYPE metadata.metadata_view;

CREATE TYPE metadata.metadata_view AS (
	data_id int8,
	source_id int4,
	source_uri text,
	source_args text,
	feed_uri text,
	feed_args text,
	data_uri text,
	data_description text,
	pending bool,
	"always" bool);

-- DROP TYPE metadata.source_binding;

CREATE TYPE metadata.source_binding AS (
	id bigserial,
	data_id int4,
	source_id int4,
	feed_args text);

-- DROP TYPE metadata.source_registry;

CREATE TYPE metadata.source_registry AS (
	id serial4,
	uri text,
	args text,
	pending bool,
	feed_id int4,
	"always" bool);

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

-- DROP TYPE metadata."_feature";

CREATE TYPE metadata."_feature" (
	INPUT = array_in,
	OUTPUT = array_out,
	RECEIVE = array_recv,
	SEND = array_send,
	ANALYZE = array_typanalyze,
	ALIGNMENT = 8,
	STORAGE = any,
	CATEGORY = A,
	ELEMENT = metadata.feature,
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

-- DROP TYPE metadata."_human_metadata_view";

CREATE TYPE metadata."_human_metadata_view" (
	INPUT = array_in,
	OUTPUT = array_out,
	RECEIVE = array_recv,
	SEND = array_send,
	ANALYZE = array_typanalyze,
	ALIGNMENT = 8,
	STORAGE = any,
	CATEGORY = A,
	ELEMENT = metadata.human_metadata_view,
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


-- metadata.feature definition

-- Drop table

-- DROP TABLE metadata.feature;

CREATE TABLE metadata.feature (
	id int8 NOT NULL,
	"label" text NULL,
	formula json NULL,
	CONSTRAINT feature_pk PRIMARY KEY (id)
);


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
	pending bool DEFAULT false NOT NULL,
	feed_id int4 NULL,
	"always" bool NULL, -- always penging. pending flag is not dropped
	CONSTRAINT sr_always_pending CHECK ((((always = true) AND (pending = true)) OR (always = false)))
);
CREATE UNIQUE INDEX source_registry_id_idx ON metadata.source_registry USING btree (id);

-- Column comments

COMMENT ON COLUMN metadata.source_registry."always" IS 'always penging. pending flag is not dropped';


-- metadata.type_registry definition

-- Drop table

-- DROP TABLE metadata.type_registry;

CREATE TABLE metadata.type_registry (
	id int4 DEFAULT nextval('metadata.index_type_id_seq'::regclass) NOT NULL,
	"name" text NOT NULL,
	description text NULL
);


-- metadata.human_metadata_view source

CREATE OR REPLACE VIEW metadata.human_metadata_view
AS SELECT dr.id,
    sr.always,
    sr.pending,
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
AS SELECT dr.id AS data_id,
    sr.id AS source_id,
    sr.uri AS source_uri,
    sr.args AS source_args,
    fr.uri AS feed_uri,
    sb.feed_args,
    dr.uri AS data_uri,
    dr.field_description AS data_description,
    sr.pending,
    sr.always
   FROM metadata.source_binding sb
     JOIN metadata.data_registry dr ON sb.data_id = dr.id
     JOIN metadata.source_registry sr ON sb.source_id = sr.id
     JOIN metadata.feed_registry fr ON sr.feed_id = fr.id
  ORDER BY dr.uri;



-- DROP PROCEDURE metadata.drop_pending_flag(int4);

CREATE OR REPLACE PROCEDURE metadata.drop_pending_flag(IN source_registry_id integer)
 LANGUAGE sql
AS $procedure$
update metadata.source_registry set pending = false where id = source_registry_id and "always" = false;
$procedure$
;

-- DROP SCHEMA "data";

CREATE SCHEMA "data" AUTHORIZATION ml_role;

-- DROP TYPE "data"."000001";

CREATE TYPE "data"."000001" AS (
	idx int8,
	f1 float8,
	f2 float8,
	f3 float8,
	f4 float8,
	f5 float8);

-- DROP TYPE "data"."000002";

CREATE TYPE "data"."000002" AS (
	idx int8,
	f1 float8,
	f2 float8,
	f3 float8,
	f4 float8,
	f5 float8);

-- DROP TYPE "data"."000003";

CREATE TYPE "data"."000003" AS (
	idx int8,
	f1 float8,
	f2 float8,
	f3 float8,
	f4 float8,
	f5 float8);

-- DROP TYPE "data"."000004";

CREATE TYPE "data"."000004" AS (
	idx int8,
	f1 float8,
	f2 float8,
	f3 float8,
	f4 float8,
	f5 float8);

-- DROP TYPE "data".master_index;

CREATE TYPE "data".master_index AS (
	idx int8);

-- DROP TYPE "data".test_linear;

CREATE TYPE "data".test_linear AS (
	idx int8,
	x float8,
	y float8);

-- DROP TYPE "data"."_000001";

CREATE TYPE "data"."_000001" (
	INPUT = array_in,
	OUTPUT = array_out,
	RECEIVE = array_recv,
	SEND = array_send,
	ANALYZE = array_typanalyze,
	ALIGNMENT = 8,
	STORAGE = any,
	CATEGORY = A,
	ELEMENT = "data"."000001",
	DELIMITER = ',');

-- DROP TYPE "data"."_000002";

CREATE TYPE "data"."_000002" (
	INPUT = array_in,
	OUTPUT = array_out,
	RECEIVE = array_recv,
	SEND = array_send,
	ANALYZE = array_typanalyze,
	ALIGNMENT = 8,
	STORAGE = any,
	CATEGORY = A,
	ELEMENT = "data"."000002",
	DELIMITER = ',');

-- DROP TYPE "data"."_000003";

CREATE TYPE "data"."_000003" (
	INPUT = array_in,
	OUTPUT = array_out,
	RECEIVE = array_recv,
	SEND = array_send,
	ANALYZE = array_typanalyze,
	ALIGNMENT = 8,
	STORAGE = any,
	CATEGORY = A,
	ELEMENT = "data"."000003",
	DELIMITER = ',');

-- DROP TYPE "data"."_000004";

CREATE TYPE "data"."_000004" (
	INPUT = array_in,
	OUTPUT = array_out,
	RECEIVE = array_recv,
	SEND = array_send,
	ANALYZE = array_typanalyze,
	ALIGNMENT = 8,
	STORAGE = any,
	CATEGORY = A,
	ELEMENT = "data"."000004",
	DELIMITER = ',');

-- DROP TYPE "data"."_master_index";

CREATE TYPE "data"."_master_index" (
	INPUT = array_in,
	OUTPUT = array_out,
	RECEIVE = array_recv,
	SEND = array_send,
	ANALYZE = array_typanalyze,
	ALIGNMENT = 8,
	STORAGE = any,
	CATEGORY = A,
	ELEMENT = "data".master_index,
	DELIMITER = ',');

-- DROP TYPE "data"."_test_linear";

CREATE TYPE "data"."_test_linear" (
	INPUT = array_in,
	OUTPUT = array_out,
	RECEIVE = array_recv,
	SEND = array_send,
	ANALYZE = array_typanalyze,
	ALIGNMENT = 8,
	STORAGE = any,
	CATEGORY = A,
	ELEMENT = "data".test_linear,
	DELIMITER = ',');
-- "data"."000001" definition

-- Drop table

-- DROP TABLE "data"."000001";

CREATE TABLE "data"."000001" (
	idx int8 NOT NULL,
	f1 float8 NULL,
	f2 float8 NULL,
	f3 float8 NULL,
	f4 float8 NULL,
	f5 float8 NULL,
	CONSTRAINT "000001_idx_key" UNIQUE (idx)
);


-- "data"."000002" definition

-- Drop table

-- DROP TABLE "data"."000002";

CREATE TABLE "data"."000002" (
	idx int8 NOT NULL,
	f1 float8 NULL,
	f2 float8 NULL,
	f3 float8 NULL,
	f4 float8 NULL,
	f5 float8 NULL,
	CONSTRAINT "000002_idx_key" UNIQUE (idx)
);


-- "data"."000003" definition

-- Drop table

-- DROP TABLE "data"."000003";

CREATE TABLE "data"."000003" (
	idx int8 NOT NULL,
	f1 float8 NULL,
	f2 float8 NULL,
	f3 float8 NULL,
	f4 float8 NULL,
	f5 float8 NULL,
	CONSTRAINT "000003_un" UNIQUE (idx)
);
CREATE UNIQUE INDEX "000003_idx_IDX" ON data."000003" USING btree (idx);


-- "data"."000004" definition

-- Drop table

-- DROP TABLE "data"."000004";

CREATE TABLE "data"."000004" (
	idx int8 NOT NULL,
	f1 float8 NULL,
	f2 float8 NULL,
	f3 float8 NULL,
	f4 float8 NULL,
	f5 float8 NULL,
	CONSTRAINT "000004_idx_key" UNIQUE (idx)
);


-- "data".master_index definition

-- Drop table

-- DROP TABLE "data".master_index;

CREATE TABLE "data".master_index (
	idx int8 NOT NULL,
	CONSTRAINT master_index_pk PRIMARY KEY (idx)
);


-- "data".test_linear definition

-- Drop table

-- DROP TABLE "data".test_linear;

CREATE TABLE "data".test_linear (
	idx int8 NOT NULL,
	x float8 NULL,
	y float8 NULL,
	CONSTRAINT test_xor_idx_key UNIQUE (idx)
);